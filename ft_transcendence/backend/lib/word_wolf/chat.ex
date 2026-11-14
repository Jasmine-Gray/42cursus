defmodule WordWolf.Chat do
  @moduledoc """
  公開チャット（`chat_messages`）の永続化コンテキスト。

  履歴取得（`list_messages/1`）は HTTP の `GET /games/{gameId}/messages` が、発言の受理
  （`publish/3`）は Channel の `chat.send` が使う。投稿経路は Channel だけで、HTTP からは投稿しない（契約）。
  """

  import Ecto.Query, warn: false

  alias WordWolf.Accounts.User
  alias WordWolf.Contract.Types
  alias WordWolf.Games.{ChatMessage, Game, GamePlayer}
  alias WordWolf.Repo
  alias WordWolf.Rooms.RoomMember

  # trim 後の許容長（契約 asyncapi.yaml: chat.send.content は minLength 1 / maxLength 300）
  @content_max 300

  @typedoc "publish/3 が返すエラー理由。Channel 側で契約のエラーコードへ写す。"
  @type publish_error ::
          :game_not_found | :not_a_player | :invalid_phase | :game_paused | :empty | :too_long

  @doc """
  ゲームの公開チャット履歴を送信時刻の昇順で返す（同時刻は id で安定化）。
  入室直後・再接続時の復元に使う。以降の新着は Channel の `chat.published`。
  """
  @spec list_messages(String.t()) :: [Types.chat_message()]
  def list_messages(game_id) when is_binary(game_id) do
    from(m in ChatMessage,
      join: u in User,
      on: u.id == m.user_id,
      where: m.game_id == ^game_id,
      order_by: [asc: m.sent_at, asc: m.id],
      select: {m.id, m.user_id, u.display_name, m.content, m.sent_at}
    )
    |> Repo.all()
    |> Enum.map(&to_contract/1)
  end

  @doc "ゲームが存在するか。"
  @spec game_exists?(String.t()) :: boolean()
  def game_exists?(game_id) when is_binary(game_id) do
    Repo.exists?(from(g in Game, where: g.id == ^game_id))
  end

  @doc """
  ユーザーがそのゲームの参加者（プレイヤーまたは観戦者）か。履歴取得の 403 判定に使う。
  発言可否（`publish/3`）はプレイヤー限定なので、こことは別の判定を使う。
  """
  @spec member?(String.t(), String.t()) :: boolean()
  def member?(game_id, user_id) when is_binary(game_id) and is_binary(user_id) do
    Repo.exists?(
      from(gp in GamePlayer, where: gp.game_id == ^game_id and gp.user_id == ^user_id)
    )
  end

  @doc """
  room チャンネル（`room:{roomId}`）の join 用。`user_id` がその部屋のメンバー
  （プレイヤー or 観戦者）なら、その部屋の**最新ゲーム ID**を返す。

  - メンバーで進行中/直近のゲームあり → `{:ok, "game_..."}`
  - メンバーだがゲーム未作成（LOBBY のみ）→ `{:ok, nil}`（join は可・送信は不可）
  - 部屋メンバーでない → `{:error, :not_a_member}`

  発言可否（プレイヤー限定・フェーズ判定）は送信時に `publish/3` が見る。ここは購読可否だけ。
  """
  @spec room_game_for(String.t(), String.t()) ::
          {:ok, String.t() | nil} | {:error, :not_a_member}
  def room_game_for(room_id, user_id) when is_binary(room_id) and is_binary(user_id) do
    if room_member?(room_id, user_id) do
      {:ok, latest_game_id(room_id)}
    else
      {:error, :not_a_member}
    end
  end

  @doc """
  発言を受理して保存し、`chat.published` に載せる形（契約 ChatMessage）で返す。

  検証順は「ゲーム存在 → プレイヤー → 送信可能フェーズ → 本文」。送信できるのはそのゲームの
  プレイヤーだけ（観戦者は不可・契約 asyncapi.yaml onChatSend）。`content` は trim してから
  1〜300 文字を検証する。`messageId` と `sentAt` はここで発行する。

  レート制限はここでは見ない（接続単位の状態なので Channel 側の責務）。
  """
  @spec publish(String.t(), String.t(), String.t()) ::
          {:ok, Types.chat_message()} | {:error, publish_error()}
  def publish(game_id, user_id, content)
      when is_binary(game_id) and is_binary(user_id) and is_binary(content) do
    with {:ok, game} <- fetch_game(game_id),
         {:ok, user} <- fetch_player(game_id, user_id),
         :ok <- ensure_sendable(game),
         {:ok, trimmed} <- validate_content(content) do
      insert_message(game, user, trimmed)
    end
  end

  # --- 内部 --------------------------------------------------------------------

  defp room_member?(room_id, user_id) do
    Repo.exists?(
      from(m in RoomMember, where: m.room_id == ^room_id and m.user_id == ^user_id)
    )
  end

  defp latest_game_id(room_id) do
    from(g in Game,
      where: g.room_id == ^room_id,
      order_by: [desc: g.started_at],
      limit: 1,
      select: g.id
    )
    |> Repo.one()
  end

  defp fetch_game(game_id) do
    case Repo.get(Game, game_id) do
      nil -> {:error, :game_not_found}
      %Game{} = game -> {:ok, game}
    end
  end

  # 送信者はそのゲームの「プレイヤー」に限る（観戦者は送信不可・契約 asyncapi.yaml onChatSend）。
  defp fetch_player(game_id, user_id) do
    query =
      from(u in User,
        join: gp in GamePlayer,
        on: gp.user_id == u.id,
        where:
          gp.game_id == ^game_id and u.id == ^user_id and
            gp.participant_type == "PLAYER"
      )

    case Repo.one(query) do
      nil -> {:error, :not_a_player}
      %User{} = user -> {:ok, user}
    end
  end

  # 送信できるのは DISCUSSION フェーズかつ一時停止していないときだけ（契約 asyncapi.yaml）。
  # フェーズの入力元は暫定で DB の games.phase。レーンA の GameServer が入ったら、
  # room_channel 側で socket.assigns のフェーズを見て判定する形へ寄せる。
  defp ensure_sendable(%Game{paused: true}), do: {:error, :game_paused}
  defp ensure_sendable(%Game{phase: "DISCUSSION"}), do: :ok
  defp ensure_sendable(%Game{}), do: {:error, :invalid_phase}

  defp validate_content(content) do
    case String.trim(content) do
      "" ->
        {:error, :empty}

      trimmed ->
        if String.length(trimmed) > @content_max do
          {:error, :too_long}
        else
          {:ok, trimmed}
        end
    end
  end

  defp insert_message(%Game{id: game_id}, %User{} = user, content) do
    {:ok, message} =
      %ChatMessage{}
      |> Ecto.Changeset.change(%{
        id: generate_id("msg"),
        game_id: game_id,
        user_id: user.id,
        content: content,
        sent_at: DateTime.utc_now()
      })
      |> Repo.insert()

    {:ok,
     to_contract({message.id, message.user_id, user.display_name, message.content, message.sent_at})}
  end

  defp to_contract({id, user_id, display_name, content, sent_at}) do
    %{
      messageId: id,
      userId: user_id,
      displayName: display_name,
      content: content,
      sentAt: DateTime.to_iso8601(sent_at)
    }
  end

  # accounts.ex の generate_id/1 と同じ方式（prefix_<URL-safe 12 文字>）。
  defp generate_id(prefix), do: prefix <> "_" <> random_string(12)

  defp random_string(length) do
    :crypto.strong_rand_bytes(length)
    |> Base.url_encode64(padding: false)
    |> binary_part(0, length)
  end
end
