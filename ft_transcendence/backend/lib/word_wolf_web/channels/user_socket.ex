defmodule WordWolfWeb.UserSocket do
  @moduledoc """
  WebSocket 接続の入口。`token`（セッションの生トークン）で認証し、`socket.assigns.user_id` を置く。
  認証できない接続は `:error`（HTTP 403）で拒否する。
  """
  use Phoenix.Socket

  alias WordWolf.Accounts

  ## チャンネル（契約 asyncapi.yaml の channels）
  channel "room:*", WordWolfWeb.RoomChannel
  channel "user:*", WordWolfWeb.UserChannel

  @impl true
  def connect(%{"token" => token}, socket, _connect_info) when is_binary(token) do
    case Accounts.get_session_by_token(token) do
      {:ok, session} ->
        {:ok,
         socket
         |> assign(:user_id, session.user.id)
         |> assign(:session_id, session.id)}

      {:error, :unauthenticated} ->
        :error
    end
  end

  def connect(_params, _socket, _connect_info), do: :error

  # 同一ユーザーの全接続を束ねる鍵。契約の connection.replaced（後の接続が前を追い出す）は
  # WordWolfWeb.Endpoint.broadcast("user_socket:" <> user_id, "disconnect", %{}) で実現できる。
  @impl true
  def id(socket), do: "user_socket:#{socket.assigns.user_id}"
end
