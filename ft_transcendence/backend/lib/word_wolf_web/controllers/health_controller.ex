defmodule WordWolfWeb.HealthController do
  # レーンC — GET /health（getHealth）。契約ファーストのリファレンス実装（_docs/dev/index.html §11）
  use WordWolfWeb, :controller

  alias WordWolf.Contract.Types

  @service "wordwolf-api"

  @spec show(Plug.Conn.t(), map()) :: Plug.Conn.t()
  def show(conn, _params) do
    json(conn, %{status: "ok", data: payload()})
  end

  # 契約とのズレは mix dialyzer が検出する
  @spec payload() :: Types.health()
  defp payload do
    %{
      service: @service,
      version: version(),
      time: DateTime.utc_now() |> DateTime.to_iso8601()
    }
  end

  defp version do
    Application.spec(:word_wolf)[:vsn] |> to_string()
  end
end
