defmodule WordWolfWeb.HealthControllerTest do
  use WordWolfWeb.ConnCase, async: true

  test "GET /health は契約どおりの封筒を返す", %{conn: conn} do
    conn = get(conn, ~p"/health")

    assert %{"status" => "ok", "data" => data} = json_response(conn, 200)
    assert %{"service" => "wordwolf-api", "version" => version, "time" => time} = data
    assert is_binary(version)
    assert {:ok, _dt, _offset} = DateTime.from_iso8601(time)
  end
end
