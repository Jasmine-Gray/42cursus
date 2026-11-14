defmodule WordWolfWeb.AuthController do
  @moduledoc """
  認証エンドポイント。成功時は `AuthSession`（生トークン + UserPrivate）を返す。
  エラーは `{:error, reason}` を返して `WordWolfWeb.FallbackController` に任せる。
  """
  use WordWolfWeb, :controller

  alias WordWolf.Accounts
  alias WordWolf.Accounts.User
  alias WordWolf.Contract.Types

  action_fallback WordWolfWeb.FallbackController

  @doc """
  POST /auth/register (register)
  ユーザーを作成し、そのままセッションを発行する（201）。
  """
  @spec register(Plug.Conn.t(), map()) :: Plug.Conn.t()
  def register(conn, params) do
    attrs = %{
      email: params["email"],
      password: params["password"],
      display_name: params["displayName"],
      terms_accepted: params["termsAccepted"]
    }

    with {:ok, user} <- Accounts.register_user(attrs),
         {:ok, {_session, token}} <- Accounts.create_session(user) do
      conn
      |> put_status(:created)
      |> json(%{status: "ok", data: auth_session(token, user)})
    end
  end

  @doc """
  POST /auth/login (login)
  メール・パスワードのどちらが誤りかは区別しない（INVALID_CREDENTIALS）。
  """
  @spec login(Plug.Conn.t(), map()) :: Plug.Conn.t()
  def login(conn, %{"email" => email, "password" => password}) do
    with {:ok, user} <- Accounts.authenticate_user(email, password),
         {:ok, {_session, token}} <- Accounts.create_session(user) do
      json(conn, %{status: "ok", data: auth_session(token, user)})
    end
  end

  def login(_conn, _params), do: {:error, :invalid_payload}

  @doc """
  POST /auth/logout (logout)
  現在のセッション（`conn.assigns.current_session`）を失効させる。冪等（常に 200）。
  同じユーザーで張っている WebSocket も切る（トークンが死ぬので再接続は `connect/3` で弾かれる）。
  """
  @spec logout(Plug.Conn.t(), map()) :: Plug.Conn.t()
  def logout(conn, _params) do
    :ok = Accounts.delete_session(conn.assigns.current_session)

    # user_socket.ex の id/1 が "user_socket:<user_id>"。connection.replaced と同じ仕組みで切断する。
    WordWolfWeb.Endpoint.broadcast("user_socket:#{conn.assigns.current_user.id}", "disconnect", %{})

    json(conn, %{status: "ok"})
  end

  @spec auth_session(String.t(), User.t()) :: Types.auth_session()
  defp auth_session(token, user) do
    %{token: token, user: Accounts.to_user_private(user)}
  end
end
