defmodule WordWolfWeb.RoomController do
  use WordWolfWeb, :controller

  alias WordWolf.Games.GameServer
  alias WordWolf.Rooms

  action_fallback WordWolfWeb.FallbackController

  def remove_member(conn, %{"roomId" => room_id, "userId" => target_user_id}) do
    with {:ok, room} <- GameServer.remove_member(room_id, conn.assigns.current_user.id, target_user_id) do
      json(conn, %{status: "ok", data: room})
    end
  end

  @doc """
  部屋設定を変更する（PUT /rooms/{roomId}/settings, updateRoomSettings）。ホスト・LOBBY中のみ。
  """
  def update_settings(conn, %{"roomId" => room_id} = params) do
    user_id = conn.assigns.current_user.id

    with {:ok, room} <- Rooms.update_settings(user_id, room_id, params) do
      json(conn, %{status: "ok", data: room})
    end
  end

  @doc """
  ゲームを開始する（POST /rooms/{roomId}/start, startGame）。

  人数・全員の準備完了・接続・進行中ゲームの不在は GameServer が検証する。
  受理後のお題と役職の決定も GameServer 側で、ここでは gameId だけ返す
  （契約どおりレスポンスに役職・お題は含めない）。開始の通知は Channel の
  phase.changed で全接続へ配信される。
  """
  def start(conn, %{"roomId" => room_id}) do
    user_id = conn.assigns.current_user.id

    case GameServer.start_game(room_id, user_id) do
      {:ok, %{gameId: game_id}} ->
        conn
        |> put_status(:created)
        |> json(%{status: "ok", data: %{gameId: game_id}})

      # LOBBY 以外＝すでに進行中。契約のエラー語彙は GAME_IN_PROGRESS
      {:error, :invalid_phase} ->
        {:error, :game_in_progress}

      {:error, reason} ->
        {:error, reason}
    end
  end

  @doc """
  部屋を作成する（POST /rooms）
  """
  def create(conn, params) do
    user_id = conn.assigns.current_user.id

    case Rooms.create_room(user_id, params) do
      {:ok, room} ->
        conn
        |> put_status(:created)
        |> json(%{status: "ok", data: room})

      {:error, :invalid_payload} ->
        {:error, :invalid_payload}
    end
  end

  @doc """
  部屋の状態を取得する（GET /rooms/:roomId）
  """
  def show(conn, %{"roomId" => room_id}) do
    user_id = conn.assigns.current_user.id

    case Rooms.get_room(user_id, room_id) do
      {:ok, room} ->
        json(conn, %{status: "ok", data: room})

      {:error, :forbidden} ->
        {:error, :forbidden}

      {:error, :not_found} ->
        {:error, :not_found}
    end
  end

  @doc """
  招待コードから部屋概要を取得する（GET /invites/:inviteCode）
  """
  def show_invite(conn, %{"inviteCode" => invite_code}) do
    case Rooms.get_preview_by_code(invite_code) do
      nil ->
        {:error, :not_found}

      preview ->
        json(conn, %{status: "ok", data: preview})
    end
  end

  @doc """
  部屋へ参加する（POST /rooms/:roomId/join）
  """
  def join(conn, %{"roomId" => room_id} = params) do
    user_id = conn.assigns.current_user.id
    participant_type = Map.get(params, "participantType", "PLAYER")

    case Rooms.join_room(user_id, room_id, participant_type) do
      {:ok, data} ->
        json(conn, %{status: "ok", data: data})

      {:error, :room_full} ->
        {:error, :conflict}

      {:error, :game_in_progress} ->
        {:error, :game_in_progress}

      {:error, :invalid_phase} ->
        {:error, :invalid_phase}

      {:error, :not_found} ->
        {:error, :not_found}

      {:error, _} ->
        {:error, :invalid_payload}
    end
  end

  @doc """
  部屋から退出する（POST /rooms/:roomId/leave）
  """
  def leave(conn, %{"roomId" => room_id}) do
    user_id = conn.assigns.current_user.id

    case Rooms.leave_room(user_id, room_id) do
      {:ok, :left} ->
        json(conn, %{status: "ok"})

      {:error, :not_found} ->
        {:error, :not_found}
    end
  end

  @doc """
  部屋への招待をフレンドへ送信する（POST /rooms/:roomId/invites）
  """
  def invite(conn, %{"roomId" => room_id, "targetUserId" => target_user_id}) do
    user_id = conn.assigns.current_user.id

    case Rooms.invite_to_room(user_id, room_id, target_user_id) do
      {:ok, invite} ->
        conn
        |> put_status(:created)
        |> json(%{status: "ok", data: invite})

      {:error, :already_joined} ->
        {:error, :conflict}

      {:error, :invalid_phase} ->
        {:error, :bad_request}

      {:error, :not_found} ->
        {:error, :not_found}

      {:error, _} ->
        {:error, :bad_request}
    end
  end

  def invite(_conn, _params), do: {:error, :bad_request}

  @doc """
  自分宛ての有効な部屋招待一覧を取得する（GET /invites）
  """
  def list_invites(conn, _params) do
    user_id = conn.assigns.current_user.id
    invites = Rooms.list_my_invites(user_id)
    json(conn, %{status: "ok", data: %{items: invites}})
  end

  @doc """
  部屋招待を承認して参加する（POST /invites/:inviteId/accept）
  """
  def accept_invite(conn, %{"inviteId" => invite_id} = params) do
    user_id = conn.assigns.current_user.id
    participant_type = Map.get(params, "participantType", "PLAYER")

    case Rooms.accept_room_invite(user_id, invite_id, participant_type) do
      {:ok, data} ->
        json(conn, %{status: "ok", data: data})

      {:error, :not_found} ->
        {:error, :not_found}

      {:error, reason} when reason in [:room_full, :conflict] ->
        {:error, :conflict}
    end
  end

  @doc """
  部屋招待を辞退する（POST /invites/:inviteId/decline）
  """
  def decline_invite(conn, %{"inviteId" => invite_id}) do
    user_id = conn.assigns.current_user.id

    case Rooms.decline_room_invite(user_id, invite_id) do
      {:ok, :declined} ->
        json(conn, %{status: "ok"})

      {:error, :not_found} ->
        {:error, :not_found}
    end
  end
end
