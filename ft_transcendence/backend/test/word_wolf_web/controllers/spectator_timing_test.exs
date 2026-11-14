defmodule WordWolfWeb.SpectatorTimingTest do
  use WordWolfWeb.ConnCase, async: false

  alias WordWolf.{Repo, Rooms}
  alias WordWolf.Games.GameServer
  alias WordWolfWeb.Endpoint

  setup do
    players = Enum.map(1..4, fn _ -> user_fixture() end)
    host = hd(players)
    room = room_fixture(host, players)
    watcher = spectator_fixture(room, user_fixture())
    server = start_supervised!({GameServer, room.id})
    word_group_fixture()

    for user <- players ++ [watcher],
        do: GameServer.connect(server, user.id, "conn_#{user.id}", self())

    %{host: host, players: players, room: room, watcher: watcher, server: server}
  end

  defp as(user) do
    {_session, token} = session_fixture(user)
    build_conn() |> authenticate_user(token)
  end

  defp start_game(ctx) do
    {:ok, %{gameId: game_id}} = GameServer.start_game(ctx.room.id, ctx.host.id)
    game_id
  end

  test "観戦者はゲーム中の全員のお題を読めるが公開状態には含めない", ctx do
    game_id = start_game(ctx)
    response = as(ctx.watcher) |> get("/games/#{game_id}/spectator-assignments")
    data = json_response(response, 200)["data"]
    state = :sys.get_state(ctx.server)
    assert data["gameId"] == game_id
    assert data["category"] == "PLACE"
    assert length(data["players"]) == 4

    for player <- data["players"] do
      assert player["word"] == state.words[player["userId"]]
      assert player["role"] == state.roles[player["userId"]]
      assert is_binary(player["displayName"])
    end

    assert get_resp_header(response, "cache-control") == ["no-store"]
    {:ok, public} = GameServer.state(ctx.server, {ctx.watcher.id, "conn_#{ctx.watcher.id}"})
    refute Enum.any?(public.players, &(Map.has_key?(&1, :word) or Map.has_key?(&1, :role)))
  end

  test "ホスト・他プレイヤー・未参加者は一覧を取得できない", ctx do
    game_id = start_game(ctx)

    for user <- ctx.players ++ [user_fixture()] do
      response = as(user) |> get("/games/#{game_id}/spectator-assignments")
      assert json_response(response, 403)["error"]["code"] == "FORBIDDEN"
    end
  end

  test "途中観戦でも取得でき、退出後は取得できない", ctx do
    game_id = start_game(ctx)
    newcomer = user_fixture()
    assert {:ok, _} = Rooms.join_room(newcomer.id, ctx.room.id, "SPECTATOR")
    assert as(newcomer) |> get("/games/#{game_id}/spectator-assignments") |> json_response(200)
    assert {:ok, _} = Rooms.leave_room(newcomer.id, ctx.room.id)
    response = as(newcomer) |> get("/games/#{game_id}/spectator-assignments")
    assert json_response(response, 403)["error"]["code"] == "FORBIDDEN"
  end

  test "ロビー復帰や次のゲーム開始と行き違った取得を拒否する", ctx do
    game_id = start_game(ctx)

    assert {:error, :not_found} =
             GenServer.call(ctx.server, {:spectator_assignments, "old_game", ctx.watcher.id})

    assert {:ok, %{gameId: ^game_id}} = GameServer.spectator_assignments(game_id, ctx.watcher.id)
  end

  test "未認証と存在しないゲームを拒否する", ctx do
    game_id = start_game(ctx)
    assert build_conn() |> get("/games/#{game_id}/spectator-assignments") |> json_response(401)
    assert as(ctx.watcher) |> get("/games/missing/spectator-assignments") |> json_response(404)
  end

  test "部屋を作ると指定した時間が保存される" do
    response =
      as(user_fixture())
      |> post("/rooms", %{
        "roomName" => "時間設定",
        "genres" => ["PLACE"],
        "discussionMs" => 90_000,
        "voteMs" => 45_000
      })

    data = json_response(response, 201)["data"]
    assert data["settings"]["minPlayers"] == 3
    assert data["settings"]["discussionMs"] == 90_000
    assert data["settings"]["voteMs"] == 45_000
    room = Repo.get!(Rooms.Room, data["roomId"])
    assert room.discussion_ms == 90_000
    assert room.vote_ms == 45_000
  end

  test "不正な時間での作成は既存の部屋を閉じずに拒否する", ctx do
    for {key, value} <- [
          {"discussionMs", 29_999},
          {"discussionMs", 900_001},
          {"discussionMs", "300000"},
          {"voteMs", 9_999},
          {"voteMs", 180_001},
          {"voteMs", nil}
        ] do
      response =
        as(ctx.host)
        |> post("/rooms", Map.put(%{"roomName" => "時間設定", "genres" => ["PLACE"]}, key, value))

      assert json_response(response, 400)["error"]["code"] == "INVALID_PAYLOAD"
    end

    assert Repo.get!(Rooms.Room, ctx.room.id).status == "WAITING"
  end

  test "時間変更は全員へ配信され、実際の議論と投票の締切に反映する", ctx do
    Endpoint.subscribe("room:#{ctx.room.id}")

    response =
      as(ctx.host)
      |> put("/rooms/#{ctx.room.id}/settings", %{
        "roomName" => ctx.room.room_name,
        "genres" => ["PLACE"],
        "discussionMs" => 90_000,
        "voteMs" => 45_000
      })

    assert json_response(response, 200)["data"]["settings"]["discussionMs"] == 90_000

    assert_receive %Phoenix.Socket.Broadcast{
      event: "room.state",
      payload: %{settings: %{discussionMs: 90_000, voteMs: 45_000}}
    }

    start_game(ctx)

    assert_receive %Phoenix.Socket.Broadcast{
                     event: "phase.changed",
                     payload: %{phase: "DISCUSSION"}
                   },
                   1_000

    state = :sys.get_state(ctx.server)
    assert DateTime.diff(state.deadline, DateTime.utc_now(), :millisecond) in 85_000..90_000
    send(ctx.server, {:phase_timeout, state.phase_version})
    state = :sys.get_state(ctx.server)
    assert state.phase == "VOTING"
    assert DateTime.diff(state.deadline, DateTime.utc_now(), :millisecond) in 40_000..45_000
  end

  test "時間の省略は現在の設定を保持する", ctx do
    response =
      as(ctx.host)
      |> put("/rooms/#{ctx.room.id}/settings", %{"roomName" => "名前だけ変更", "genres" => ["PLACE"]})

    data = json_response(response, 200)["data"]
    assert data["settings"]["discussionMs"] == ctx.room.discussion_ms
    assert data["settings"]["voteMs"] == ctx.room.vote_ms
  end

  test "ホスト以外とゲーム開始後の設定変更を拒否する", ctx do
    params = %{
      "roomName" => ctx.room.room_name,
      "genres" => ["PLACE"],
      "discussionMs" => 90_000,
      "voteMs" => 45_000
    }

    response = as(ctx.watcher) |> put("/rooms/#{ctx.room.id}/settings", params)
    assert json_response(response, 403)["error"]["code"] == "NOT_HOST"
    start_game(ctx)
    response = as(ctx.host) |> put("/rooms/#{ctx.room.id}/settings", params)
    assert json_response(response, 409)["error"]["code"] == "INVALID_PHASE"
    assert :sys.get_state(ctx.server).settings.discussion_ms == ctx.room.discussion_ms
  end

  test "時間範囲外・小数・文字列への変更は保存されない", ctx do
    for value <- [0, 900_001, 30_000.5, "90000", nil] do
      response =
        as(ctx.host)
        |> put("/rooms/#{ctx.room.id}/settings", %{
          "roomName" => ctx.room.room_name,
          "genres" => ["PLACE"],
          "discussionMs" => value
        })

      assert json_response(response, 400)["error"]["code"] == "INVALID_PAYLOAD"
    end

    assert Repo.get!(Rooms.Room, ctx.room.id).discussion_ms == ctx.room.discussion_ms
  end

  test "ジャンルと逆転モードを同期し次のゲームのお題へ適用する", ctx do
    group = word_group_fixture()
    group |> Ecto.Changeset.change(genre: "FOOD") |> Repo.update!()
    Endpoint.subscribe("room:#{ctx.room.id}")

    response =
      as(ctx.host)
      |> put("/rooms/#{ctx.room.id}/settings", %{
        "roomName" => ctx.room.room_name,
        "genres" => ["FOOD"],
        "comebackEnabled" => true
      })

    settings = json_response(response, 200)["data"]["settings"]
    assert settings["genres"] == ["FOOD"]
    assert settings["comebackEnabled"]

    assert_receive %Phoenix.Socket.Broadcast{
      event: "room.state",
      payload: %{settings: %{genres: ["FOOD"], comebackEnabled: true}}
    }

    start_game(ctx)
    state = :sys.get_state(ctx.server)
    assert state.word_group.genre == "FOOD"
    assert state.settings.comeback_enabled
  end

  test "ホストがプレイヤーと観戦者を退出させ一覧と接続を更新する", ctx do
    Endpoint.subscribe("room:#{ctx.room.id}")

    for target <- [Enum.at(ctx.players, 1), ctx.watcher] do
      response = as(ctx.host) |> delete("/rooms/#{ctx.room.id}/members/#{target.id}")
      data = json_response(response, 200)["data"]
      refute Enum.any?(data["players"] ++ data["spectators"], &(&1["userId"] == target.id))
      refute Repo.get_by(Rooms.RoomMember, room_id: ctx.room.id, user_id: target.id)
      connection_id = "conn_#{target.id}"
      assert_receive {:room_removed, ^connection_id}
      assert_receive %Phoenix.Socket.Broadcast{event: "room.state", payload: snapshot}
      refute Enum.any?(snapshot.players ++ snapshot.spectators, &(&1.userId == target.id))

      assert {:error, :stale_connection} =
               GameServer.state(ctx.server, {target.id, connection_id})

      assert {:error, :forbidden} = GameServer.connect(ctx.server, target.id, "retry", self())
      assert as(target) |> get("/rooms/#{ctx.room.id}") |> json_response(403)
    end

    assert Repo.get!(Rooms.Room, ctx.room.id).host_user_id == ctx.host.id
    assert Repo.get!(Rooms.Room, ctx.room.id).status == "WAITING"
  end

  test "ホスト以外と未認証の退出操作を拒否する", ctx do
    target = Enum.at(ctx.players, 1)
    url = "/rooms/#{ctx.room.id}/members/#{target.id}"
    assert build_conn() |> delete(url) |> json_response(401)

    for actor <- [target, ctx.watcher, user_fixture()] do
      assert as(actor) |> delete(url) |> json_response(403)
    end

    assert Repo.get_by(Rooms.RoomMember, room_id: ctx.room.id, user_id: target.id)
  end

  test "自分自身・存在しない参加者・ゲーム中の退出操作を拒否する", ctx do
    assert as(ctx.host)
           |> delete("/rooms/#{ctx.room.id}/members/#{ctx.host.id}")
           |> json_response(400)

    assert as(ctx.host) |> delete("/rooms/#{ctx.room.id}/members/missing") |> json_response(404)
    assert as(ctx.host) |> delete("/rooms/missing/members/missing") |> json_response(404)
    start_game(ctx)
    target = Enum.at(ctx.players, 1)

    assert as(ctx.host)
           |> delete("/rooms/#{ctx.room.id}/members/#{target.id}")
           |> json_response(409)

    assert Repo.get_by(Rooms.RoomMember, room_id: ctx.room.id, user_id: target.id)
    assert Map.has_key?(:sys.get_state(ctx.server).connections, target.id)
  end

  test "ロビーを開いていない退出対象者にも本人用チャンネルで通知する", ctx do
    target = ctx.watcher
    GameServer.disconnect(ctx.server, {target.id, "conn_#{target.id}"})
    Endpoint.subscribe("user:#{target.id}")
    Endpoint.subscribe("user:#{ctx.host.id}")
    response = as(ctx.host) |> delete("/rooms/#{ctx.room.id}/members/#{target.id}")
    assert json_response(response, 200)
    topic = "user:#{target.id}"

    assert_receive %Phoenix.Socket.Broadcast{
      topic: ^topic,
      event: "room.removed",
      payload: payload
    }

    assert payload == %{
             roomId: ctx.room.id,
             roomName: ctx.room.room_name,
             reason: "REMOVED_BY_HOST"
           }

    refute_receive %Phoenix.Socket.Broadcast{event: "room.removed"}
  end

  test "キック後に再招待でき、招待済みでも同じ招待を再通知して再参加できる", ctx do
    target = Enum.at(ctx.players, 1)

    assert as(ctx.host)
           |> delete("/rooms/#{ctx.room.id}/members/#{target.id}")
           |> json_response(200)

    Endpoint.subscribe("user:#{target.id}")
    url = "/rooms/#{ctx.room.id}/invites"
    first = as(ctx.host) |> post(url, %{"targetUserId" => target.id}) |> json_response(201)
    assert_receive %Phoenix.Socket.Broadcast{event: "room.invited", payload: invite}
    second = as(ctx.host) |> post(url, %{"targetUserId" => target.id}) |> json_response(201)
    assert_receive %Phoenix.Socket.Broadcast{event: "room.invited", payload: resent}
    assert first["data"]["inviteId"] == second["data"]["inviteId"]
    assert invite.inviteId == resent.inviteId

    response =
      as(target) |> post("/invites/#{invite.inviteId}/accept", %{"participantType" => "PLAYER"})

    assert json_response(response, 200)
    assert Repo.get_by(Rooms.RoomMember, room_id: ctx.room.id, user_id: target.id)
  end
end
