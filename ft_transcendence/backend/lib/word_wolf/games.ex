defmodule WordWolf.Games do
  @moduledoc false
  # レーンC — ゲーム結果の永続化（契約: openapi.yaml の games タグ）。進行は Games.GameServer（レーンA）
  #
  # いまは画面04 ホーム（GET /users/me の stats）が使う参照系のみ。

  import Ecto.Query, warn: false
  alias WordWolf.Games.{Game, GamePlayer, Vote}
  alias WordWolf.Repo

  @type summary :: %{
          gamesPlayed: non_neg_integer(),
          wins: non_neg_integer(),
          losses: non_neg_integer()
        }

  @doc """
  ユーザーの戦績サマリ。決着済み（phase = "RESULT"）のゲームで、プレイヤーとして
  参加したものだけを数える。勝率は契約上返さない（クライアントで導出）。
  """
  @spec summary_for(String.t()) :: summary()
  def summary_for(user_id) do
    rows =
      from(gp in GamePlayer,
        join: g in Game,
        on: g.id == gp.game_id,
        where:
          gp.user_id == ^user_id and gp.participant_type == "PLAYER" and
            g.phase == "RESULT",
        select: {gp.role, g.winner}
      )
      |> Repo.all()

    played = length(rows)
    wins = Enum.count(rows, fn {role, winner} -> won?(role, winner) end)

    %{gamesPlayed: played, wins: wins, losses: played - wins}
  end

  # 市民は市民側勝利で、ウルフはウルフ側勝利で「勝ち」。
  defp won?("CITIZEN", "CITIZEN_SIDE"), do: true
  defp won?("WORD_WOLF", "WOLF_SIDE"), do: true
  defp won?(_role, _winner), do: false

  @doc """
  対戦履歴（GET /users/me/games, listMyGames）。決着済み（phase = "RESULT"）で
  中止されていない（aborted_at が nil）ゲームを新しい順に返す。契約 MatchHistoryItem。
  """
  @spec history_for(String.t(), pos_integer(), pos_integer()) :: %{items: [map()], meta: map()}
  def history_for(user_id, page, limit) do
    base =
      from(gp in GamePlayer,
        join: g in Game,
        on: g.id == gp.game_id,
        join: r in WordWolf.Rooms.Room,
        on: r.id == g.room_id,
        where:
          gp.user_id == ^user_id and gp.participant_type == "PLAYER" and
            g.phase == "RESULT" and is_nil(g.aborted_at)
      )

    total = Repo.aggregate(base, :count)

    items =
      base
      |> order_by([_gp, g, _r], desc: g.started_at)
      |> offset(^((page - 1) * limit))
      |> limit(^limit)
      |> select([gp, g, r], %{
        gameId: g.id,
        roomName: r.room_name,
        role: gp.role,
        winner: g.winner,
        playedAt: g.started_at
      })
      |> Repo.all()
      |> Enum.map(fn row ->
        %{
          gameId: row.gameId,
          roomName: row.roomName,
          role: row.role,
          outcome: if(won?(row.role, row.winner), do: "WIN", else: "LOSS"),
          playedAt: DateTime.to_iso8601(row.playedAt)
        }
      end)

    %{items: items, meta: %{total: total, page: page, limit: limit}}
  end

  ## 書き込み（GameServer から呼ぶ）------------------------------------------

  @doc """
  ゲーム開始時に games と game_players を1トランザクションで作る。

  `roles` / `words` はプレイヤーIDをキーにしたマップ、`spectator_ids` は観戦者。
  DB制約 game_players_assignment_check により、PLAYER は role と assigned_word の
  両方が必要で、SPECTATOR は両方 NULL でなければならない。
  """
  @spec create_game(map()) :: {:ok, Game.t()} | {:error, term()}
  def create_game(%{
        game_id: game_id,
        room_id: room_id,
        word_group_id: word_group_id,
        phase: phase,
        phase_version: phase_version,
        deadline: deadline,
        roles: roles,
        words: words,
        spectator_ids: spectator_ids
      }) do
    now = DateTime.utc_now()

    players =
      Enum.map(roles, fn {user_id, role} ->
        game_player_attrs(game_id, user_id, "PLAYER", role, Map.get(words, user_id), now)
      end)

    spectators =
      Enum.map(spectator_ids, fn user_id ->
        game_player_attrs(game_id, user_id, "SPECTATOR", nil, nil, now)
      end)

    game_attrs = %{
      id: game_id,
      room_id: room_id,
      word_group_id: word_group_id,
      phase: phase,
      phase_version: phase_version,
      deadline: deadline,
      paused: false,
      started_at: now,
      inserted_at: now,
      updated_at: now
    }

    Repo.transaction(fn ->
      from(r in WordWolf.Rooms.Room, where: r.id == ^room_id)
      |> Repo.update_all(set: [status: "IN_GAME", updated_at: now])

      {1, _} = Repo.insert_all(Game, [game_attrs])
      {_count, _} = Repo.insert_all(GamePlayer, players ++ spectators)
      Repo.get!(Game, game_id)
    end)
  end

  @doc "途中から接続した観戦者もゲーム参加者として記録する。お題・役職は持たない。"
  @spec record_spectator(String.t(), String.t()) :: :ok
  def record_spectator(game_id, user_id) do
    Repo.insert_all(
      GamePlayer,
      [game_player_attrs(game_id, user_id, "SPECTATOR", nil, nil, DateTime.utc_now())],
      on_conflict: :nothing,
      conflict_target: [:game_id, :user_id]
    )

    :ok
  end

  defp game_player_attrs(game_id, user_id, participant_type, role, assigned_word, now) do
    %{
      id: "gp_#{game_id}_#{user_id}",
      game_id: game_id,
      user_id: user_id,
      participant_type: participant_type,
      role: role,
      assigned_word: assigned_word,
      connected: true,
      inserted_at: now,
      updated_at: now
    }
  end

  @doc "フェーズ遷移を写す。RESULT に入ったときだけ ended_at を打つ。"
  @spec update_phase(String.t(), String.t(), non_neg_integer(), DateTime.t() | nil) :: :ok
  def update_phase(game_id, phase, phase_version, deadline) do
    fields = [phase: phase, phase_version: phase_version, deadline: deadline]
    fields = if phase == "RESULT", do: [{:ended_at, DateTime.utc_now()} | fields], else: fields

    update_game(game_id, fields)
  end

  @doc "切断待機の開始・解除を写す。"
  @spec set_paused(String.t(), boolean(), DateTime.t() | nil) :: :ok
  def set_paused(game_id, paused, reconnect_deadline),
    do: update_game(game_id, paused: paused, reconnect_deadline: reconnect_deadline)

  @doc "勝敗を確定する。winner / result_reason は DB制約の語彙に限られる。"
  @spec record_result(
          String.t(),
          String.t(),
          String.t(),
          WordWolf.Contract.Types.comeback_result() | nil
        ) :: :ok
  def record_result(game_id, winner, result_reason, comeback_result \\ nil) do
    update_game(game_id,
      winner: winner,
      result_reason: result_reason,
      comeback_result: comeback_result,
      ended_at: DateTime.utc_now()
    )
  end

  @doc "再接続待ちが尽きて流れたゲームに aborted_at を打つ。"
  @spec abort_game(String.t()) :: :ok
  def abort_game(game_id), do: update_game(game_id, aborted_at: DateTime.utc_now())

  @doc "1票を記録する。二重投票は unique_index(:votes, [:game_id, :voter_user_id]) で弾く。"
  @spec record_vote(String.t(), String.t(), String.t()) :: :ok
  def record_vote(game_id, voter_user_id, target_user_id) do
    now = DateTime.utc_now()

    Repo.insert_all(
      Vote,
      [
        %{
          id: "vote_#{game_id}_#{voter_user_id}",
          game_id: game_id,
          voter_user_id: voter_user_id,
          target_user_id: target_user_id,
          voted_at: now,
          inserted_at: now
        }
      ],
      on_conflict: :nothing
    )

    :ok
  end

  @doc "接続状態を写す。ゲーム中でなければ何もしない。"
  @spec set_player_connected(String.t(), String.t(), boolean()) :: :ok
  def set_player_connected(game_id, user_id, connected) do
    from(gp in GamePlayer, where: gp.game_id == ^game_id and gp.user_id == ^user_id)
    |> Repo.update_all(set: [connected: connected, updated_at: DateTime.utc_now()])

    :ok
  end

  defp update_game(game_id, fields) do
    from(g in Game, where: g.id == ^game_id)
    |> Repo.update_all(set: [{:updated_at, DateTime.utc_now()} | fields])

    :ok
  end
end
