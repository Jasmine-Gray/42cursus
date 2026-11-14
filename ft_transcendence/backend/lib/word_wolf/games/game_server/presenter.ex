defmodule WordWolf.Games.GameServer.Presenter do
  @moduledoc false

  alias WordWolf.Games.GameServer.State

  @spec public_state(State.t()) :: map()
  def public_state(state) do
    %{
      roomId: state.room_id,
      gameId: state.game_id,
      phase: state.phase,
      comeback: if(state.phase == "COMEBACK", do: state.comeback, else: nil),
      phaseVersion: state.phase_version,
      deadline: iso(state.deadline),
      paused: state.paused,
      reconnectDeadline: iso(state.reconnect_deadline),
      hostUserId: state.host_user_id,
      settings: %{
        genres: state.settings.genres,
        comebackEnabled: state.settings.comeback_enabled,
        discussionMs: state.settings.discussion_ms,
        voteMs: state.settings.vote_ms
      },
      players: Enum.map(state.players, &public_player/1),
      spectators: Enum.map(state.spectators, &public_spectator/1),
      skipProgress: if(state.phase == "DISCUSSION", do: skip_progress(state), else: nil),
      voteProgress: if(state.phase == "VOTING", do: vote_progress(state), else: nil)
    }
  end

  @doc """
  REST の GET /games/{gameId}（契約 GamePublicState）。room.state と違い hostUserId と
  spectators 配列を持たず、startedAt と spectatorCount を返す。
  """
  @spec game_public_state(State.t()) :: map()
  def game_public_state(state) do
    %{
      gameId: state.game_id,
      roomId: state.room_id,
      phase: state.phase,
      comeback: if(state.phase == "COMEBACK", do: state.comeback, else: nil),
      phaseVersion: state.phase_version,
      deadline: iso(state.deadline),
      paused: state.paused,
      reconnectDeadline: iso(state.reconnect_deadline),
      startedAt: iso(state.started_at),
      players: Enum.map(state.players, &game_player/1),
      spectatorCount: map_size(state.spectators),
      voteProgress: if(state.phase == "VOTING", do: vote_progress(state), else: nil)
    }
  end

  @doc """
  REST の GET /games/{gameId}/result（契約 GameResult）。確定済みの結果に、
  閲覧者自身の投票先（myVote）だけを足す。観戦者・未投票は null。
  """
  @spec result_for(State.t(), String.t()) :: map() | nil
  def result_for(%{result: nil}, _user_id), do: nil

  def result_for(state, user_id) do
    my_vote =
      case Map.fetch(state.votes, user_id) do
        {:ok, target} -> %{targetUserId: target}
        :error -> nil
      end

    Map.put(state.result, :myVote, my_vote)
  end

  def private_state(state, user_id) do
    case Map.fetch(state.words, user_id) do
      {:ok, word} -> {:ok, %{gameId: state.game_id, word: word, category: state.word_group.genre}}
      :error -> {:ok, nil}
    end
  end

  @spec spectator_assignments(State.t()) :: WordWolf.Contract.Types.spectator_assignments()
  def spectator_assignments(state) do
    players =
      state.players
      |> Enum.sort_by(fn {id, _player} -> id end)
      |> Enum.map(fn {id, player} ->
        %{
          userId: id,
          displayName: player.display_name,
          avatarUrl: player.avatar_url,
          word: Map.fetch!(state.words, id),
          role: Map.fetch!(state.roles, id)
        }
      end)

    %{gameId: state.game_id, category: state.word_group.genre, players: players}
  end

  def game_result(state, counts, wolf_id, sole_max) do
    %{
      gameId: state.game_id,
      winner: if(sole_max, do: "CITIZEN_SIDE", else: "WOLF_SIDE"),
      reason: if(sole_max, do: "WOLF_IDENTIFIED", else: "WOLF_ESCAPED"),
      comeback: state.comeback_result,
      wolfUserId: wolf_id,
      words: %{
        majority: %{word: majority_word(state), count: map_size(state.players) - 1},
        minority: %{word: Map.fetch!(state.words, wolf_id), count: 1}
      },
      players: Enum.map(state.players, &result_player(&1, state, counts))
    }
  end

  def majority_word(state) do
    user_id = Enum.find_value(state.roles, fn {id, role} -> if role == "CITIZEN", do: id end)
    Map.fetch!(state.words, user_id)
  end

  def vote_progress(state),
    do: %{votedCount: map_size(state.votes), playerCount: map_size(state.players)}

  @doc "議論スキップの賛成人数（契約 SkipProgress）。誰が賛成したかは出さない。"
  def skip_progress(state),
    do: %{skippedCount: MapSet.size(state.skip_votes), playerCount: map_size(state.players)}

  def iso(nil), do: nil
  def iso(datetime), do: DateTime.to_iso8601(datetime)

  defp public_player({_id, player}) do
    %{
      userId: player.user_id,
      displayName: player.display_name,
      avatarUrl: player.avatar_url,
      ready: player.ready,
      connected: player.connected
    }
  end

  defp public_spectator({_id, spectator}) do
    %{
      userId: spectator.user_id,
      displayName: spectator.display_name,
      avatarUrl: spectator.avatar_url
    }
  end

  # 契約 ResultPlayer は UserPublic（userId/displayName/avatarUrl）を含む
  defp result_player({id, player}, state, counts) do
    %{
      userId: id,
      displayName: player.display_name,
      avatarUrl: player.avatar_url,
      role: Map.fetch!(state.roles, id),
      word: Map.fetch!(state.words, id),
      votesReceived: Map.get(counts, id, 0)
    }
  end

  # 契約 GamePlayer は UserPublic + connected（ready は持たない）
  defp game_player({_id, player}) do
    %{
      userId: player.user_id,
      displayName: player.display_name,
      avatarUrl: player.avatar_url,
      connected: player.connected
    }
  end
end
