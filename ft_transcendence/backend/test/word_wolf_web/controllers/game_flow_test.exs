defmodule WordWolfWeb.GameFlowTest do
  use WordWolfWeb.ConnCase, async: false

  alias WordWolf.Games.{GameServer, GameSupervisor}
  alias WordWolfWeb.Endpoint

  setup %{conn: conn} do
    on_exit(&stop_game_servers/0)

    players = Enum.map(1..4, fn _ -> user_fixture() end)
    host = hd(players)
    # DISCUSSION を最短にして VOTING まで進められるようにする
    room = room_fixture(host, players, %{discussion_ms: 10, vote_ms: 10_000})
    word_group_fixture()

    {:ok, server} = GameSupervisor.ensure_started(room.id)

    tokens =
      Map.new(players, fn user ->
        {_session, token} = session_fixture(user)
        GameServer.connect(server, user.id, "conn_#{user.id}", self())
        {user.id, token}
      end)

    %{conn: conn, players: players, host: host, room: room, server: server, tokens: tokens}
  end

  defp as(conn, user, %{tokens: tokens}), do: authenticate_user(conn, tokens[user.id])

  describe "POST /rooms/{roomId}/start" do
    test "ホストはゲームを開始できる", %{conn: conn, host: host, room: room} = ctx do
      conn = conn |> as(host, ctx) |> post("/rooms/#{room.id}/start")

      assert %{"status" => "ok", "data" => %{"gameId" => game_id}} = json_response(conn, 201)
      assert String.starts_with?(game_id, "game_")
    end

    test "ホスト以外は NOT_HOST", %{conn: conn, players: players, room: room} = ctx do
      other = Enum.at(players, 1)
      conn = conn |> as(other, ctx) |> post("/rooms/#{room.id}/start")

      assert %{"error" => %{"code" => "NOT_HOST"}} = json_response(conn, 403)
    end

    test "進行中に再度開始すると GAME_IN_PROGRESS",
         %{conn: conn, host: host, room: room} = ctx do
      assert conn |> as(host, ctx) |> post("/rooms/#{room.id}/start") |> json_response(201)

      conn = build_conn() |> as(host, ctx) |> post("/rooms/#{room.id}/start")
      assert %{"error" => %{"code" => "GAME_IN_PROGRESS"}} = json_response(conn, 409)
    end

    test "準備できていなければ NOT_READY", %{conn: conn, host: host, room: room} = ctx do
      # 1人の準備を外す
      other = Enum.at(ctx.players, 1)
      :ok = WordWolf.Rooms.persist_member_ready(room.id, other.id, false)
      GameServer.set_ready(ctx.server, {other.id, "conn_#{other.id}"}, false)

      conn = conn |> as(host, ctx) |> post("/rooms/#{room.id}/start")
      assert %{"error" => %{"code" => "NOT_READY"}} = json_response(conn, 409)
    end
  end

  describe "ゲーム開始後の参照系" do
    setup %{conn: conn, host: host, room: room} = ctx do
      %{"data" => %{"gameId" => game_id}} =
        conn |> as(host, ctx) |> post("/rooms/#{room.id}/start") |> json_response(201)

      %{game_id: game_id}
    end

    test "GET /games/{gameId} が公開状態を返す", %{conn: conn, host: host, game_id: id} = ctx do
      conn = conn |> as(host, ctx) |> get("/games/#{id}")
      assert %{"status" => "ok", "data" => data} = json_response(conn, 200)

      # 契約 GamePublicState の必須項目
      for key <- ~w(gameId roomId phase phaseVersion deadline paused reconnectDeadline
                    startedAt players spectatorCount voteProgress) do
        assert Map.has_key?(data, key), "#{key} が無い"
      end

      assert data["gameId"] == id
      assert length(data["players"]) == 4
      # 公開状態に秘密は含めない
      refute Enum.any?(data["players"], &Map.has_key?(&1, "role"))
      refute Enum.any?(data["players"], &Map.has_key?(&1, "word"))
    end

    test "GET /games/{gameId}/assignment が本人のお題だけ返す",
         %{conn: conn, host: host, game_id: id} = ctx do
      conn = conn |> as(host, ctx) |> get("/games/#{id}/assignment")
      assert %{"status" => "ok", "data" => data} = json_response(conn, 200)

      assert data["gameId"] == id
      assert data["word"] in ["図書館", "本屋"]
      assert data["category"] == "PLACE"
      # 役職は返さない（契約 Assignment）
      refute Map.has_key?(data, "role")
    end

    test "観戦者の assignment は NOT_A_PLAYER", %{conn: conn, room: room, game_id: id} = ctx do
      watcher = spectator_fixture(room, user_fixture())
      {_session, token} = session_fixture(watcher)
      GameServer.connect(ctx.server, watcher.id, "conn_#{watcher.id}", self())

      conn = conn |> authenticate_user(token) |> get("/games/#{id}/assignment")
      assert %{"error" => %{"code" => "NOT_A_PLAYER"}} = json_response(conn, 403)
    end

    test "部屋の参加者でなければ FORBIDDEN", %{conn: conn, game_id: id} do
      outsider = user_fixture()
      {_session, token} = session_fixture(outsider)

      conn = conn |> authenticate_user(token) |> get("/games/#{id}")
      assert %{"error" => %{"code" => "FORBIDDEN"}} = json_response(conn, 403)
    end

    test "結果は RESULT フェーズ前だと INVALID_PHASE",
         %{conn: conn, host: host, game_id: id} = ctx do
      conn = conn |> as(host, ctx) |> get("/games/#{id}/result")
      assert %{"error" => %{"code" => "INVALID_PHASE"}} = json_response(conn, 409)
    end
  end

  describe "POST /games/{gameId}/votes" do
    setup %{conn: conn, host: host, room: room} = ctx do
      Endpoint.subscribe("room:#{room.id}")

      %{"data" => %{"gameId" => game_id}} =
        conn |> as(host, ctx) |> post("/rooms/#{room.id}/start") |> json_response(201)

      # DEALING(10ms) → DISCUSSION(10ms) → VOTING
      assert_receive %Phoenix.Socket.Broadcast{
                       event: "phase.changed",
                       payload: %{phase: "VOTING"}
                     },
                     1_000

      %{game_id: game_id}
    end

    test "投票が受理され進捗が返る", %{conn: conn, host: host, players: players, game_id: id} = ctx do
      target = List.last(players)
      conn = conn |> as(host, ctx) |> post("/games/#{id}/votes", %{"targetUserId" => target.id})

      assert %{"status" => "ok", "data" => %{"votedCount" => 1, "playerCount" => 4}} =
               json_response(conn, 201)
    end

    test "二重投票は ALREADY_SUBMITTED",
         %{conn: conn, host: host, players: players, game_id: id} = ctx do
      target = List.last(players)
      body = %{"targetUserId" => target.id}
      assert conn |> as(host, ctx) |> post("/games/#{id}/votes", body) |> json_response(201)

      conn = build_conn() |> as(host, ctx) |> post("/games/#{id}/votes", body)
      assert %{"error" => %{"code" => "ALREADY_SUBMITTED"}} = json_response(conn, 409)
    end

    test "自分自身への投票は INVALID_TARGET", %{conn: conn, host: host, game_id: id} = ctx do
      conn = conn |> as(host, ctx) |> post("/games/#{id}/votes", %{"targetUserId" => host.id})
      assert %{"error" => %{"code" => "INVALID_TARGET"}} = json_response(conn, 400)
    end

    test "targetUserId が無ければ INVALID_PAYLOAD", %{conn: conn, host: host, game_id: id} = ctx do
      conn = conn |> as(host, ctx) |> post("/games/#{id}/votes", %{})
      assert %{"error" => %{"code" => "INVALID_PAYLOAD"}} = json_response(conn, 400)
    end

    test "全員が投票すると結果を取得できる",
         %{conn: conn, host: host, players: players, game_id: id} = ctx do
      target = List.last(players)

      Enum.each(players, fn player ->
        to = if player.id == target.id, do: host.id, else: target.id

        build_conn()
        |> as(player, ctx)
        |> post("/games/#{id}/votes", %{"targetUserId" => to})
        |> json_response(201)
      end)

      conn = conn |> as(host, ctx) |> get("/games/#{id}/result")
      assert %{"status" => "ok", "data" => data} = json_response(conn, 200)

      assert data["gameId"] == id
      assert data["winner"] in ["CITIZEN_SIDE", "WOLF_SIDE"]
      assert data["reason"] in ["WOLF_IDENTIFIED", "WOLF_ESCAPED"]
      assert data["myVote"]["targetUserId"] == target.id
      # 契約 ResultPlayer は UserPublic を含む
      assert Enum.all?(data["players"], &Map.has_key?(&1, "displayName"))
      assert Enum.all?(data["players"], &Map.has_key?(&1, "votesReceived"))
    end
  end
end
