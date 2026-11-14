defmodule WordWolfWeb.RoomChannelTest do
  use WordWolfWeb.ChannelCase, async: false

  alias WordWolf.Games.GameServer
  alias WordWolfWeb.{RoomChannel, UserSocket}

  setup do
    # サンドボックスのオーナーが降りる前に GameServer を畳む（game_fixtures.ex の注意書き）
    on_exit(&stop_game_servers/0)

    players = Enum.map(1..4, fn _ -> user_fixture() end)
    room = room_fixture(hd(players), players)
    %{players: players, room: room}
  end

  defp join_room(user, room) do
    socket = socket(UserSocket, "socket:#{user.id}", %{user_id: user.id})
    subscribe_and_join(socket, RoomChannel, "room:#{room.id}")
  end

  # 全プレイヤーを接続させてゲームを開始し、DISCUSSION まで進めて各socketを返す。
  # start_game は全員が connected かつ ready であることを要求する（ready_to_start/1）。
  defp discussion(players, room) do
    sockets =
      Map.new(players, fn user ->
        {:ok, _reply, socket} = join_room(user, room)
        {user.id, socket}
      end)

    word_group_fixture()
    assert {:ok, %{gameId: _}} = GameServer.start_game(room.id, hd(players).id)
    assert_broadcast "phase.changed", %{phase: "DISCUSSION"}

    sockets
  end

  test "逆転の公開入力と確定回答をroomチャンネルで扱う", %{players: players, room: room} do
    room = room |> Ecto.Changeset.change(comeback_enabled: true) |> WordWolf.Repo.update!()
    sockets = discussion(players, room)
    server = Map.fetch!(sockets, hd(players).id).assigns.game_server
    state = :sys.get_state(server)
    send(server, {:phase_timeout, state.phase_version})
    state = :sys.get_state(server)
    wolf_id = Enum.find_value(state.roles, fn {id, role} -> if role == "WORD_WOLF", do: id end)
    citizen_id = Enum.find_value(state.roles, fn {id, role} -> if role == "CITIZEN", do: id end)

    Enum.each(players, fn player ->
      target = if player.id == wolf_id, do: citizen_id, else: wolf_id
      assert {:ok, _} = GameServer.cast_vote(state.game_id, player.id, target)
    end)

    assert_broadcast "phase.changed", %{phase: "COMEBACK"}
    wolf = Map.fetch!(sockets, wolf_id)
    citizen = Map.fetch!(sockets, citizen_id)
    ref = push(citizen, "comeback.draft", %{"content" => "他人の回答"})
    assert_reply ref, :error, %{error: %{code: "FORBIDDEN"}}
    ref = push(wolf, "comeback.draft", %{"content" => "入力中"})
    assert_reply ref, :ok, %{content: "入力中"}
    assert_broadcast "comeback.updated", %{content: "入力中", wolfUserId: ^wolf_id}
    ref = push(wolf, "comeback.submit", %{"content" => "答え", "userId" => wolf_id})
    assert_reply ref, :error, %{error: %{code: "INVALID_PAYLOAD"}}
    answer = WordWolf.Games.GameServer.Presenter.majority_word(state)
    ref = push(wolf, "comeback.submit", %{"content" => answer})
    assert_reply ref, :ok, %{status: "ok"}
    assert_broadcast "result.published", %{reason: "WOLF_GUESSED"}
  end

  test "退出した本人へ通知してチャンネルを閉じ、残ったホストは操作できる", %{players: [host, target | _], room: room} do
    start_supervised!({GameServer, room.id})
    {:ok, _, host_socket} = join_room(host, room)
    {:ok, _, target_socket} = join_room(target, room)
    monitor = Process.monitor(target_socket.channel_pid)
    assert {:ok, _} = GameServer.remove_member(room.id, host.id, target.id)
    assert_push "room.removed", %{reason: "REMOVED_BY_HOST"}
    assert_receive {:DOWN, ^monitor, :process, _, :normal}
    assert {:error, %{error: %{code: "FORBIDDEN"}}} = join_room(target, room)
    ref = push(host_socket, "state.request", %{})
    assert_reply ref, :ok, %{publicState: state}
    refute Enum.any?(state.players, &(&1.userId == target.id))
  end

  describe "join" do
    test "member can join and receives a public snapshot", %{players: [user | _], room: room} do
      assert {:ok, reply, _socket} = join_room(user, room)
      assert reply.status == "ok"
      assert is_binary(reply.connectionId)
      assert reply.publicState.roomId == room.id
      refute Map.has_key?(reply.publicState, :word)
    end

    test "non-member cannot join", %{room: room} do
      outsider = user_fixture()

      assert {:error, %{status: "error", error: %{code: "FORBIDDEN"}}} = join_room(outsider, room)
    end

    test "new connection replaces the old connection", %{players: [user | _], room: room} do
      first = socket(UserSocket, "socket:first", %{user_id: user.id})
      assert {:ok, _, _} = subscribe_and_join(first, RoomChannel, "room:#{room.id}")

      second = socket(UserSocket, "socket:second", %{user_id: user.id})
      assert {:ok, _, _} = subscribe_and_join(second, RoomChannel, "room:#{room.id}")
      assert_push "connection.replaced", %{reason: "NEWER_CONNECTION"}
    end
  end

  describe "chat.send" do
    test "clientMessageId が無い payload は INVALID_PAYLOAD", %{players: [user | _], room: room} do
      {:ok, _reply, socket} = join_room(user, room)

      ref = push(socket, "chat.send", %{"content" => "id が無い"})
      assert_reply ref, :error, %{status: "error", error: %{code: "INVALID_PAYLOAD"}}
      refute_broadcast "chat.published", _
    end

    test "DISCUSSION 以外のフェーズは INVALID_PHASE", %{players: [user | _], room: room} do
      {:ok, _reply, socket} = join_room(user, room)

      ref = push(socket, "chat.send", %{"clientMessageId" => "c-1", "content" => "まだ話せる？"})
      assert_reply ref, :error, %{status: "error", error: %{code: "INVALID_PHASE"}}
      refute_broadcast "chat.published", _
    end

    test "プレイヤーの発言を受理し、chat.published を全員へ broadcast する", context do
      %{players: [sender | _] = players, room: room} = context
      sockets = discussion(players, room)

      ref =
        push(sockets[sender.id], "chat.send", %{
          "clientMessageId" => "c-1",
          "content" => "  こんばんは  "
        })

      assert_reply ref, :ok, %{status: "ok", messageId: message_id}
      assert String.starts_with?(message_id, "msg_")

      assert_broadcast "chat.published", payload
      assert payload.clientMessageId == "c-1"
      assert payload.messageId == message_id
      assert payload.userId == sender.id
      assert payload.displayName == sender.display_name
      # 契約どおり trim してから配信する（asyncapi.yaml chatSend content）
      assert payload.content == "こんばんは"
      assert is_binary(payload.sentAt)
    end

    test "観戦者は NOT_A_PLAYER で拒否される", %{players: players, room: room} do
      watcher = spectator_fixture(room, user_fixture())
      _sockets = discussion(players, room)
      {:ok, _reply, socket} = join_room(watcher, room)

      ref = push(socket, "chat.send", %{"clientMessageId" => "c-1", "content" => "なるほど"})
      assert_reply ref, :error, %{status: "error", error: %{code: "NOT_A_PLAYER"}}
      refute_broadcast "chat.published", _
    end

    test "1 秒以内の連続送信は 2 件目が RATE_LIMITED（接続は切れない）", context do
      %{players: [sender | _] = players, room: room} = context
      sockets = discussion(players, room)
      socket = sockets[sender.id]

      ref1 = push(socket, "chat.send", %{"clientMessageId" => "c-1", "content" => "1 通目"})
      assert_reply ref1, :ok, %{status: "ok", messageId: _}

      ref2 = push(socket, "chat.send", %{"clientMessageId" => "c-2", "content" => "2 通目"})
      assert_reply ref2, :error, %{status: "error", error: %{code: "RATE_LIMITED"}}

      # 接続は生きていて、次の handle_in も処理される
      ref3 = push(socket, "chat.send", %{"clientMessageId" => "c-3", "content" => "3 通目"})
      assert_reply ref3, :error, %{status: "error", error: %{code: "RATE_LIMITED"}}
    end

    test "同じ clientMessageId の再送は重複として弾く", context do
      %{players: [sender | _] = players, room: room} = context
      sockets = discussion(players, room)
      socket = sockets[sender.id]

      ref1 = push(socket, "chat.send", %{"clientMessageId" => "dup", "content" => "1 回目"})
      assert_reply ref1, :ok, %{status: "ok", messageId: _}

      # レート制限に先んじて重複排除が効く（契約 chatSend clientMessageId「重複排除に使う」）
      ref2 = push(socket, "chat.send", %{"clientMessageId" => "dup", "content" => "2 回目"})
      assert_reply ref2, :error, %{status: "error", error: %{code: "INVALID_PAYLOAD"}}
    end
  end

  # c/friends-invite（Rooms.set_player_ready 版）から移植。ready の正本が GameServer へ
  # 移ったので、DBシードではなく fixture ベースで検証する。
  describe "room.ready" do
    test "LOBBY 中のプレイヤーが ready を切り替えると room.state が流れる", context do
      # 先頭はホスト（常に準備完了）なので、ホスト以外で切り替える
      %{players: [_host, user | _], room: room} = context
      {:ok, _reply, socket} = join_room(user, room)

      ref = push(socket, "room.ready", %{"ready" => false})
      assert_reply ref, :ok, %{status: "ok", ready: false}

      assert_broadcast "room.state", payload
      assert payload.roomId == room.id
      assert Enum.find(payload.players, &(&1.userId == user.id)).ready == false

      ref2 = push(socket, "room.ready", %{"ready" => true})
      assert_reply ref2, :ok, %{status: "ok", ready: true}

      assert_broadcast "room.state", payload2
      assert Enum.find(payload2.players, &(&1.userId == user.id)).ready == true
    end

    test "ready はDBにも書き写される（REST が古い値を返さないため）", context do
      %{players: [_host, user | _], room: room} = context
      {:ok, _reply, socket} = join_room(user, room)

      ref = push(socket, "room.ready", %{"ready" => false})
      assert_reply ref, :ok, %{status: "ok", ready: false}

      member =
        WordWolf.Repo.get_by!(WordWolf.Rooms.RoomMember, room_id: room.id, user_id: user.id)

      refute member.ready
    end

    test "ホストが ready: false を送っても準備完了のまま（契約 onRoomReady）", context do
      %{players: [host | _], room: room} = context
      {:ok, _reply, socket} = join_room(host, room)

      ref = push(socket, "room.ready", %{"ready" => false})
      assert_reply ref, :ok, %{status: "ok", ready: true}

      assert_broadcast "room.state", payload
      assert Enum.find(payload.players, &(&1.userId == host.id)).ready == true
    end

    test "観戦者は NOT_A_PLAYER で拒否される", %{players: players, room: room} do
      watcher = spectator_fixture(room, user_fixture())
      {:ok, _reply, socket} = join_room(watcher, room)
      _ = players

      ref = push(socket, "room.ready", %{"ready" => true})
      assert_reply ref, :error, %{status: "error", error: %{code: "NOT_A_PLAYER"}}
    end

    test "ready が boolean でなければ INVALID_PAYLOAD", context do
      %{players: [user | _], room: room} = context
      {:ok, _reply, socket} = join_room(user, room)

      ref = push(socket, "room.ready", %{"ready" => "yes"})
      assert_reply ref, :error, %{status: "error", error: %{code: "INVALID_PAYLOAD"}}
    end
  end

  describe "discussion.skip" do
    test "全員が賛成すると締切を待たずVOTINGへ進む", context do
      %{players: players, room: room} = context
      sockets = discussion(players, room)

      # 最後の1人の前までは DISCUSSION のまま
      players
      |> Enum.drop(-1)
      |> Enum.each(fn player ->
        ref = push(sockets[player.id], "discussion.skip", %{"skip" => true})
        assert_reply ref, :ok, %{status: "ok", skip: true}
      end)

      assert_broadcast "skip.progress", %{skippedCount: 3, playerCount: 4}
      refute_broadcast "phase.changed", %{phase: "VOTING"}

      ref = push(sockets[List.last(players).id], "discussion.skip", %{"skip" => true})
      assert_reply ref, :ok, %{status: "ok", skip: true}

      assert_broadcast "phase.changed", %{phase: "VOTING"}
    end

    test "賛成は取り消せる", context do
      %{players: [user | _], room: room} = context
      sockets = discussion(context.players, room)

      ref = push(sockets[user.id], "discussion.skip", %{"skip" => true})
      assert_reply ref, :ok, %{status: "ok", skip: true}
      assert_broadcast "skip.progress", %{skippedCount: 1}

      ref2 = push(sockets[user.id], "discussion.skip", %{"skip" => false})
      assert_reply ref2, :ok, %{status: "ok", skip: false}
      assert_broadcast "skip.progress", %{skippedCount: 0}
    end

    test "同じ値を送り直しても冪等", context do
      %{players: [user | _], room: room} = context
      sockets = discussion(context.players, room)

      for _ <- 1..3 do
        ref = push(sockets[user.id], "discussion.skip", %{"skip" => true})
        assert_reply ref, :ok, %{status: "ok", skip: true}
      end

      assert_broadcast "skip.progress", %{skippedCount: 1, playerCount: 4}
    end

    test "観戦者は NOT_A_PLAYER", %{players: players, room: room} do
      watcher = spectator_fixture(room, user_fixture())
      _sockets = discussion(players, room)
      {:ok, _reply, socket} = join_room(watcher, room)

      ref = push(socket, "discussion.skip", %{"skip" => true})
      assert_reply ref, :error, %{status: "error", error: %{code: "NOT_A_PLAYER"}}
    end

    test "DISCUSSION 以外は INVALID_PHASE", %{players: [user | _], room: room} do
      {:ok, _reply, socket} = join_room(user, room)

      ref = push(socket, "discussion.skip", %{"skip" => true})
      assert_reply ref, :error, %{status: "error", error: %{code: "INVALID_PHASE"}}
    end

    test "skip が boolean でなければ INVALID_PAYLOAD", context do
      %{players: [user | _], room: room} = context
      sockets = discussion(context.players, room)

      ref = push(sockets[user.id], "discussion.skip", %{"skip" => "yes"})
      assert_reply ref, :error, %{status: "error", error: %{code: "INVALID_PAYLOAD"}}
    end

    test "VOTING へ移ると賛成状態は破棄される", context do
      %{players: players, room: room} = context
      sockets = discussion(players, room)

      Enum.each(players, fn player ->
        push(sockets[player.id], "discussion.skip", %{"skip" => true})
      end)

      assert_broadcast "phase.changed", %{phase: "VOTING"}

      ref = push(sockets[hd(players).id], "state.request", %{})
      assert_reply ref, :ok, %{status: "ok", publicState: public_state}
      # DISCUSSION 以外では skipProgress は null（契約 RoomStateSnapshot）
      assert public_state.skipProgress == nil
    end
  end

  describe "state.request" do
    test "全フェーズで公開状態を返す", %{players: [user | _], room: room} do
      {:ok, _reply, socket} = join_room(user, room)

      ref = push(socket, "state.request", %{})
      assert_reply ref, :ok, %{status: "ok", publicState: public_state}
      assert public_state.roomId == room.id
    end
  end
end
