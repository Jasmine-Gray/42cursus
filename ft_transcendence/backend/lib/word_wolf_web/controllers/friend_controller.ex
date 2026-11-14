defmodule WordWolfWeb.FriendController do
  @moduledoc """
  フレンドおよびフレンド申請（`/friends`）のエンドポイント。
  `conn.assigns.current_user` は router の `:authenticated` パイプラインが置く。
  """
  use WordWolfWeb, :controller

  alias WordWolf.Friends

  action_fallback WordWolfWeb.FallbackController

  @doc """
  GET /friends (listFriends)
  フレンド一覧（オンライン状態スナップショット付き）を取得する。
  """
  @spec index(Plug.Conn.t(), map()) :: Plug.Conn.t()
  def index(conn, _params) do
    user_id = conn.assigns.current_user.id
    friends = Friends.list_friends(user_id)
    json(conn, %{status: "ok", data: %{items: friends}})
  end

  @doc """
  GET /friends/requests (listFriendRequests)
  未処理のフレンド申請一覧（受信または送信）を取得する。
  """
  @spec list_requests(Plug.Conn.t(), map()) :: Plug.Conn.t()
  def list_requests(conn, params) do
    user_id = conn.assigns.current_user.id
    direction = Map.get(params, "direction", "incoming")
    requests = Friends.list_friend_requests(user_id, direction)
    json(conn, %{status: "ok", data: %{items: requests}})
  end

  @doc """
  POST /friends/requests (sendFriendRequest)
  指定したユーザーへフレンド申請を送信する。
  """
  @spec create_request(Plug.Conn.t(), map()) :: Plug.Conn.t()
  def create_request(conn, %{"targetUserId" => target_user_id}) when is_binary(target_user_id) do
    user_id = conn.assigns.current_user.id

    with {:ok, data} <- Friends.send_friend_request(user_id, target_user_id) do
      conn
      |> put_status(:created)
      |> json(%{status: "ok", data: data})
    end
  end

  def create_request(conn, _params) do
    conn
    |> put_status(:bad_request)
    |> json(%{
      status: "error",
      error: %{
        code: "INVALID_PAYLOAD",
        message: "targetUserId is required."
      }
    })
  end

  @doc """
  POST /friends/requests/:requestId/accept (acceptFriendRequest)
  フレンド申請を承認する。
  """
  @spec accept_request(Plug.Conn.t(), map()) :: Plug.Conn.t()
  def accept_request(conn, %{"requestId" => request_id}) when is_binary(request_id) do
    user_id = conn.assigns.current_user.id

    with {:ok, friend} <- Friends.accept_friend_request(user_id, request_id) do
      json(conn, %{status: "ok", data: friend})
    end
  end

  @doc """
  POST /friends/requests/:requestId/decline (declineFriendRequest)
  フレンド申請を拒否する。
  """
  @spec decline_request(Plug.Conn.t(), map()) :: Plug.Conn.t()
  def decline_request(conn, %{"requestId" => request_id}) when is_binary(request_id) do
    user_id = conn.assigns.current_user.id

    with {:ok, :declined} <- Friends.decline_friend_request(user_id, request_id) do
      json(conn, %{status: "ok"})
    end
  end

  @doc """
  DELETE /friends/:userId (removeFriend)
  フレンドを解除する。
  """
  @spec delete(Plug.Conn.t(), map()) :: Plug.Conn.t()
  def delete(conn, %{"userId" => friend_user_id}) when is_binary(friend_user_id) do
    user_id = conn.assigns.current_user.id

    with :ok <- Friends.remove_friend(user_id, friend_user_id) do
      json(conn, %{status: "ok"})
    end
  end
end
