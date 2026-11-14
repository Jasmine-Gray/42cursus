defmodule WordWolfWeb.GameControllerTest do
  use WordWolfWeb.ConnCase, async: true

  # 開発シード（WordWolf.DevSeeds）を使う。
  #   game_history_001 … RESULT。user_yu はプレイヤー。チャット msg_1..msg_4（昇順）。
  #   game_history_002 … RESULT。user_yu はプレイヤー。チャット無し。
  #   game_case_099    … VOTING。user_yu は不参加（観戦者は user_mei）。
  #   user_yu のセッション token は dev-token-yu。
  describe "GET /games/:game_id/messages" do
    setup %{conn: conn} do
      WordWolf.DevSeeds.run()
      {:ok, conn: authenticate_user(conn, "dev-token-yu")}
    end

    test "参加ゲームのチャット履歴を送信時刻の昇順で封筒形式で返す", %{conn: conn} do
      conn = get(conn, ~p"/games/game_history_001/messages")

      assert %{"status" => "ok", "data" => %{"items" => items}} = json_response(conn, 200)
      assert length(items) == 4

      assert Enum.map(items, & &1["content"]) == [
               "静かに過ごすことが多い場所ですよね。",
               "私は休日にもよく行きます。長居することもあります。",
               "そこで何かを借りることはありますか？",
               "借りることもありますが、読むだけの時もあります。"
             ]

      first = hd(items)
      assert first["messageId"] == "msg_1"
      assert first["userId"] == "user_aoi"
      assert first["displayName"] == "アオイ"
      assert is_binary(first["sentAt"])
      assert first |> Map.keys() |> Enum.sort() == ~w(content displayName messageId sentAt userId)
    end

    test "チャットが無いゲームでは items は空配列", %{conn: conn} do
      conn = get(conn, ~p"/games/game_history_002/messages")
      assert %{"status" => "ok", "data" => %{"items" => []}} = json_response(conn, 200)
    end

    test "参加していないゲームは 403 FORBIDDEN", %{conn: conn} do
      conn = get(conn, ~p"/games/game_case_099/messages")
      assert %{"status" => "error", "error" => %{"code" => "FORBIDDEN"}} = json_response(conn, 403)
    end

    test "存在しないゲームは 404 NOT_FOUND", %{conn: conn} do
      conn = get(conn, ~p"/games/game_missing/messages")
      assert %{"status" => "error", "error" => %{"code" => "NOT_FOUND"}} = json_response(conn, 404)
    end
  end

  test "未認証は 401 UNAUTHENTICATED", %{conn: conn} do
    conn = get(conn, ~p"/games/game_history_001/messages")
    assert %{"status" => "error", "error" => %{"code" => "UNAUTHENTICATED"}} = json_response(conn, 401)
  end
end
