defmodule WordWolf.Presence do
  @moduledoc """
  ユーザーのオンライン状態（`ONLINE` / `IN_GAME` / `OFFLINE`）の判定・更新・配信。

  状態そのものは `users.presence_state` / `users.last_seen_at` に写像し（GET /friends は
  ここを読むだけ）、実際の判定材料は `users.ws_connection_count`（user チャンネルの生存接続数）と
  `game_players` / `games`（終了していないゲームの PLAYER か）の2つ。
  """

  import Ecto.Query, warn: false

  alias WordWolf.Accounts.User
  alias WordWolf.Friends
  alias WordWolf.Games.GamePlayer
  alias WordWolf.Repo

  @doc "user チャンネルへの接続を1つ増やす。0→1（最初の接続）ならプレゼンスを再計算・配信する。"
  @spec connect(String.t()) :: :ok
  def connect(user_id) when is_binary(user_id) do
    case bump_connection_count(user_id, 1) do
      1 -> recompute_and_broadcast(user_id)
      _ -> :ok
    end

    :ok
  end

  @doc "user チャンネルの切断。1→0（最後の接続）ならプレゼンスを再計算・配信する。"
  @spec disconnect(String.t()) :: :ok
  def disconnect(user_id) when is_binary(user_id) do
    case bump_connection_count(user_id, -1) do
      0 -> recompute_and_broadcast(user_id)
      _ -> :ok
    end

    :ok
  end

  @doc "ゲーム開始時に呼ぶ。渡した全員（PLAYER）を IN_GAME へ再計算・配信する。"
  @spec game_started([String.t()]) :: :ok
  def game_started(player_user_ids) when is_list(player_user_ids) do
    Enum.each(player_user_ids, &recompute_and_broadcast/1)
  end

  @doc "ゲーム終了（結果確定・中止）時に呼ぶ。渡した全員を ONLINE/OFFLINE へ戻す。"
  @spec game_ended([String.t()]) :: :ok
  def game_ended(player_user_ids) when is_list(player_user_ids) do
    Enum.each(player_user_ids, &recompute_and_broadcast/1)
  end

  @doc """
  そのユーザーがいま入るべき状態を判定する（`IN_GAME` > `ONLINE` > `OFFLINE` の優先順）。
  DB を実際に更新したいときは `recompute_and_broadcast/1` を使う。
  """
  @spec resolve_state(String.t()) :: String.t()
  def resolve_state(user_id) when is_binary(user_id) do
    cond do
      in_active_game?(user_id) -> "IN_GAME"
      connected?(user_id) -> "ONLINE"
      true -> "OFFLINE"
    end
  end

  @doc "resolve_state/1 の結果で users.presence_state を更新し、変化があればフレンドへ配信する。"
  @spec recompute_and_broadcast(String.t()) :: :ok
  def recompute_and_broadcast(user_id) when is_binary(user_id) do
    new_state = resolve_state(user_id)

    case Repo.get(User, user_id) do
      nil -> :ok
      %User{presence_state: ^new_state} -> :ok
      %User{} = user -> apply_state(user, new_state)
    end

    :ok
  end

  # --- 内部 --------------------------------------------------------------------

  defp connected?(user_id) do
    Repo.exists?(from(u in User, where: u.id == ^user_id and u.ws_connection_count > 0))
  end

  # 「終了していないゲームの PLAYER」＝ ended_at・aborted_at がどちらも nil。
  # SPECTATOR は対象外（契約上の状態遷移は観戦を IN_GAME に含めない）。
  defp in_active_game?(user_id) do
    Repo.exists?(
      from(gp in GamePlayer,
        join: g in assoc(gp, :game),
        where:
          gp.user_id == ^user_id and gp.participant_type == "PLAYER" and
            is_nil(g.ended_at) and is_nil(g.aborted_at)
      )
    )
  end

  defp apply_state(%User{} = user, new_state) do
    last_seen_at = if new_state == "OFFLINE", do: DateTime.utc_now(), else: user.last_seen_at

    user
    |> Ecto.Changeset.change(presence_state: new_state, last_seen_at: last_seen_at)
    |> Repo.update()
    |> case do
      {:ok, _updated} -> broadcast_change(user.id, new_state)
      {:error, _} -> :ok
    end
  end

  defp broadcast_change(user_id, state) do
    user_id
    |> Friends.friend_ids()
    |> Enum.each(fn friend_id ->
      WordWolfWeb.Endpoint.broadcast("user:#{friend_id}", "friend.presence_changed", %{
        userId: user_id,
        state: state
      })
    end)
  end

  # 接続数の増減はDB側で GREATEST(…, 0) にクランプする（disconnect の二重呼び出しなどで
  # 負数にならないようにするための保険）。返り値は更新後の件数。
  #
  # 注意: update_all の :returning オプションは insert/insert_all 用で update_all には無い。
  # 更新後の値を受け取るには select を付けた「更新クエリ」にする必要がある
  # （Ecto.Repo.update_all/3 のドキュメント: "unless a select is supplied in the update query"）。
  defp bump_connection_count(user_id, delta) when delta in [1, -1] do
    query =
      from(u in User,
        where: u.id == ^user_id,
        select: u.ws_connection_count,
        update: [
          set: [
            ws_connection_count: fragment("GREATEST(? + ?, 0)", u.ws_connection_count, ^delta),
            updated_at: ^DateTime.utc_now()
          ]
        ]
      )

    case Repo.update_all(query, []) do
      {1, [count]} -> count
      _ -> nil
    end
  end
end
