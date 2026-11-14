defmodule WordWolfWeb.Plugs.Authenticate do
  @moduledoc """
  Bearer トークンによる認証 Plug。
  Authorization: Bearer <token> を検証し、conn.assigns に :current_user と :current_session をセットする。
  失敗時は 401 UNAUTHENTICATED エラーを返す。
  """

  import Plug.Conn
  import Phoenix.Controller, only: [json: 2]

  alias WordWolf.Accounts

  def init(opts), do: opts

  def call(conn, _opts) do
    case get_bearer_token(conn) do
      {:ok, token} ->
        case Accounts.get_session_by_token(token) do
          {:ok, session} ->
            conn
            |> assign(:current_session, session)
            |> assign(:current_user, session.user)

          {:error, :unauthenticated} ->
            unauthenticated_response(conn, "Invalid or expired session token.")
        end

      {:error, :no_token} ->
        unauthenticated_response(conn, "Authentication required.")
    end
  end

  defp get_bearer_token(conn) do
    case get_req_header(conn, "authorization") do
      ["Bearer " <> token] when byte_size(token) > 0 -> {:ok, String.trim(token)}
      _ -> {:error, :no_token}
    end
  end

  defp unauthenticated_response(conn, message) do
    conn
    |> put_status(:unauthorized)
    |> json(%{
      status: "error",
      error: %{
        code: "UNAUTHENTICATED",
        message: message
      }
    })
    |> halt()
  end
end
