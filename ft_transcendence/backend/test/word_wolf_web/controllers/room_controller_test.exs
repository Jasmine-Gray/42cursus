defmodule WordWolfWeb.RoomControllerTest do
  use WordWolfWeb.ConnCase, async: true

  alias WordWolf.DevSeeds

  setup do
    DevSeeds.run()
    :ok
  end

  describe "POST /rooms" do
    test "部屋を作成できる", %{conn: conn} do
      conn =
        conn
        |> put_req_header("authorization", "Bearer dev-token-yu")
        |> post("/rooms", %{
          "roomName" => "テストの部屋",
          "genres" => ["PLACE", "FOOD"],
          "comebackEnabled" => false
        })

      assert %{
               "status" => "ok",
               "data" => %{
                 "roomId" => room_id,
                 "roomName" => "テストの部屋",
                 "status" => "WAITING",
                 "inviteCode" => invite_code,
                 "hostUserId" => "user_yu",
                 "players" => players
               }
             } = json_response(conn, 201)

      assert String.starts_with?(room_id, "room_")
      assert String.starts_with?(invite_code, "CASE-")
      assert length(players) == 1
      assert hd(players)["userId"] == "user_yu"
      assert hd(players)["isHost"] == true
      assert hd(players)["ready"] == true
    end

    test "削除した恋愛と本音・価値観では部屋を作成できない", %{conn: conn} do
      conn =
        conn
        |> put_req_header("authorization", "Bearer dev-token-yu")
        |> post("/rooms", %{
          "roomName" => "本音の捜査会議",
          "genres" => ["ROMANCE", "VALUES"]
        })

      assert json_response(conn, 400)["error"]["code"] == "INVALID_PAYLOAD"
    end

    test "不正なパラメータの場合は 400", %{conn: conn} do
      conn =
        conn
        |> put_req_header("authorization", "Bearer dev-token-yu")
        |> post("/rooms", %{
          "roomName" => "",
          "genres" => []
        })

      assert json_response(conn, 400)["status"] == "error"
    end
  end

  describe "GET /rooms/:roomId" do
    test "参加者が部屋情報を取得できる", %{conn: conn} do
      conn =
        conn
        |> put_req_header("authorization", "Bearer dev-token-yu")
        |> get("/rooms/room_case_042")

      assert %{
               "status" => "ok",
               "data" => %{
                 "roomId" => "room_case_042",
                 "roomName" => "深夜の読書会",
                 "status" => "WAITING",
                 "players" => players
               }
             } = json_response(conn, 200)

      assert length(players) >= 1
    end

    test "未参加者が取得しようとすると 403", %{conn: conn} do
      # メイは初期状態では room_case_042 に未参加
      conn =
        conn
        |> put_req_header("authorization", "Bearer dev-token-mei")
        |> get("/rooms/room_case_042")

      assert json_response(conn, 403)["status"] == "error"
    end
  end

  describe "GET /invites/:inviteCode" do
    test "招待コードから部屋概要を取得できる", %{conn: conn} do
      conn =
        conn
        |> put_req_header("authorization", "Bearer dev-token-mei")
        |> get("/invites/CASE-042")

      assert %{
               "status" => "ok",
               "data" => %{
                 "roomId" => "room_case_042",
                 "inviteCode" => "CASE-042",
                 "roomName" => "深夜の読書会",
                 "status" => "WAITING",
                 "playerCount" => count,
                 "maxPlayers" => 8,
                 "joinable" => true
               }
             } = json_response(conn, 200)

      assert is_integer(count)
    end

    test "存在しない招待コードの場合は 404", %{conn: conn} do
      conn =
        conn
        |> put_req_header("authorization", "Bearer dev-token-mei")
        |> get("/invites/NON-EXISTENT")

      assert json_response(conn, 404)["status"] == "error"
    end
  end

  describe "POST /rooms/:roomId/join" do
    test "プレイヤーとして参加できる", %{conn: conn} do
      conn =
        conn
        |> put_req_header("authorization", "Bearer dev-token-mei")
        |> post("/rooms/room_case_042/join", %{"participantType" => "PLAYER"})

      assert %{
               "status" => "ok",
               "data" => %{
                 "room" => %{
                   "roomId" => "room_case_042",
                   "players" => players
                 },
                 "realtime" => %{
                   "roomTopic" => "room:room_case_042",
                   "userTopic" => "user:user_mei"
                 }
               }
             } = json_response(conn, 200)

      assert Enum.any?(players, &(&1["userId"] == "user_mei"))
    end

    test "観戦者として参加できる", %{conn: conn} do
      conn =
        conn
        |> put_req_header("authorization", "Bearer dev-token-mei")
        |> post("/rooms/room_case_042/join", %{"participantType" => "SPECTATOR"})

      assert %{
               "status" => "ok",
               "data" => %{
                 "room" => %{
                   "roomId" => "room_case_042",
                   "spectators" => spectators
                 },
                 "realtime" => %{
                   "roomTopic" => "room:room_case_042",
                   "userTopic" => "user:user_mei"
                 }
               }
             } = json_response(conn, 200)

      assert Enum.any?(spectators, &(&1["userId"] == "user_mei"))
    end
  end

  describe "POST /rooms/:roomId/invites" do
    test "部屋の参加者がフレンドへ部屋招待を送信できる", %{conn: conn} do
      # 田中ユウ(user_yu) は room_case_042 に参加中。
      # メイ(user_mei) を招待する。
      conn =
        conn
        |> put_req_header("authorization", "Bearer dev-token-yu")
        |> post("/rooms/room_case_042/invites", %{"targetUserId" => "user_mei"})

      assert %{
               "status" => "ok",
               "data" => %{
                 "inviteId" => invite_id,
                 "roomId" => "room_case_042",
                 "inviteCode" => "CASE-042",
                 "roomName" => "深夜の読書会",
                 "from" => %{
                   "userId" => "user_yu",
                   "displayName" => "田中 ユウ"
                 }
               }
             } = json_response(conn, 201)

      assert String.starts_with?(invite_id, "rinv_")
    end

    test "すでに部屋に参加しているユーザーを招待した場合は 409", %{conn: conn} do
      # ミナト(user_minato) はすでに room_case_042 に参加中
      conn =
        conn
        |> put_req_header("authorization", "Bearer dev-token-yu")
        |> post("/rooms/room_case_042/invites", %{"targetUserId" => "user_minato"})

      assert json_response(conn, 409)["status"] == "error"
    end
  end

  describe "GET /invites" do
    test "自分宛ての有効な部屋招待一覧を取得できる", %{conn: conn} do
      # 田中ユウがメイに招待を送信
      conn
      |> put_req_header("authorization", "Bearer dev-token-yu")
      |> post("/rooms/room_case_042/invites", %{"targetUserId" => "user_mei"})

      # メイが招待一覧を取得
      mei_conn =
        build_conn()
        |> put_req_header("authorization", "Bearer dev-token-mei")
        |> get("/invites")

      assert %{
               "status" => "ok",
               "data" => %{
                 "items" => [
                   %{
                     "roomId" => "room_case_042",
                     "roomName" => "深夜の読書会",
                     "from" => %{
                       "userId" => "user_yu"
                     }
                   }
                 ]
               }
             } = json_response(mei_conn, 200)
    end
  end

  describe "POST /invites/:inviteId/accept" do
    test "招待を承認して部屋に参加できる", %{conn: conn} do
      # 田中ユウがメイに招待を送信
      invite_res =
        conn
        |> put_req_header("authorization", "Bearer dev-token-yu")
        |> post("/rooms/room_case_042/invites", %{"targetUserId" => "user_mei"})
        |> json_response(201)

      invite_id = invite_res["data"]["inviteId"]

      # メイが招待を承認
      mei_conn =
        build_conn()
        |> put_req_header("authorization", "Bearer dev-token-mei")
        |> post("/invites/#{invite_id}/accept", %{"participantType" => "PLAYER"})

      assert %{
               "status" => "ok",
               "data" => %{
                 "room" => %{
                   "roomId" => "room_case_042",
                   "players" => players
                 },
                 "realtime" => %{
                   "roomTopic" => "room:room_case_042",
                   "userTopic" => "user:user_mei"
                 }
               }
             } = json_response(mei_conn, 200)

      assert Enum.any?(players, &(&1["userId"] == "user_mei"))
    end
  end

  describe "POST /invites/:inviteId/decline" do
    test "招待を辞退できる", %{conn: conn} do
      # 田中ユウがメイに招待を送信
      invite_res =
        conn
        |> put_req_header("authorization", "Bearer dev-token-yu")
        |> post("/rooms/room_case_042/invites", %{"targetUserId" => "user_mei"})
        |> json_response(201)

      invite_id = invite_res["data"]["inviteId"]

      # メイが招待を辞退
      mei_conn =
        build_conn()
        |> put_req_header("authorization", "Bearer dev-token-mei")
        |> post("/invites/#{invite_id}/decline")

      assert json_response(mei_conn, 200)["status"] == "ok"

      # 一覧から消えていることを確認
      list_conn =
        build_conn()
        |> put_req_header("authorization", "Bearer dev-token-mei")
        |> get("/invites")

      assert json_response(list_conn, 200)["data"]["items"] == []
    end

    test "ホストが新しい部屋を作成した場合、古い部屋の招待は無効になり参加できない", %{conn: conn} do
      # 1. 田中ユウがメイに room_case_042 への招待を送信
      invite_res =
        conn
        |> put_req_header("authorization", "Bearer dev-token-yu")
        |> post("/rooms/room_case_042/invites", %{"targetUserId" => "user_mei"})
        |> json_response(201)

      invite_id = invite_res["data"]["inviteId"]

      # 2. 田中ユウが新しい部屋を作成
      yu_conn =
        build_conn()
        |> put_req_header("authorization", "Bearer dev-token-yu")
        |> post("/rooms", %{
          "roomName" => "新しい部屋",
          "genres" => ["PLACE", "FOOD"]
        })

      assert json_response(yu_conn, 201)["status"] == "ok"

      # 3. メイが招待一覧を取得すると、古い部屋の招待は消えている
      list_conn =
        build_conn()
        |> put_req_header("authorization", "Bearer dev-token-mei")
        |> get("/invites")

      assert json_response(list_conn, 200)["data"]["items"] == []

      # 4. メイが古い招待で参加しようとしても 404 で拒否される
      accept_conn =
        build_conn()
        |> put_req_header("authorization", "Bearer dev-token-mei")
        |> post("/invites/#{invite_id}/accept", %{"participantType" => "PLAYER"})

      assert json_response(accept_conn, 404)["status"] == "error"
    end
  end

  describe "POST /rooms/:roomId/leave" do
    test "部屋から退出できる", %{conn: conn} do
      # ミナト(user_minato) が room_case_042 から退出
      conn =
        conn
        |> put_req_header("authorization", "Bearer dev-token-minato")
        |> post("/rooms/room_case_042/leave")

      assert json_response(conn, 200)["status"] == "ok"

      # 再度退出しようとすると 404
      conn2 =
        build_conn()
        |> put_req_header("authorization", "Bearer dev-token-minato")
        |> post("/rooms/room_case_042/leave")

      assert json_response(conn2, 404)["status"] == "error"
    end

    test "観戦者も部屋から退出できる", %{conn: conn} do
      # メイ(user_mei) が観戦者として参加
      join_conn =
        conn
        |> put_req_header("authorization", "Bearer dev-token-mei")
        |> post("/rooms/room_case_042/join", %{"participantType" => "SPECTATOR"})

      assert json_response(join_conn, 200)["status"] == "ok"

      # 観戦者メイが退出
      leave_conn =
        build_conn()
        |> put_req_header("authorization", "Bearer dev-token-mei")
        |> post("/rooms/room_case_042/leave")

      assert json_response(leave_conn, 200)["status"] == "ok"

      # 再度退出しようとすると 404
      leave_again =
        build_conn()
        |> put_req_header("authorization", "Bearer dev-token-mei")
        |> post("/rooms/room_case_042/leave")

      assert json_response(leave_again, 404)["status"] == "error"
    end

    test "ホストが退出すると次の最古参プレイヤーへホストが移譲される", %{conn: conn} do
      # 田中ユウ(user_yu) は room_case_042 のホスト
      yu_conn =
        conn
        |> put_req_header("authorization", "Bearer dev-token-yu")
        |> post("/rooms/room_case_042/leave")

      assert json_response(yu_conn, 200)["status"] == "ok"

      # 残ったプレイヤー（例: user_aoi または user_minato）が部屋情報を取得し、新ホストが設定されていることを確認
      aoi_conn =
        build_conn()
        |> put_req_header("authorization", "Bearer dev-token-aoi")
        |> get("/rooms/room_case_042")

      res = json_response(aoi_conn, 200)
      assert res["status"] == "ok"
      assert res["data"]["hostUserId"] != "user_yu"
      assert Enum.any?(res["data"]["players"], &(&1["isHost"] == true))
    end
  end
end
