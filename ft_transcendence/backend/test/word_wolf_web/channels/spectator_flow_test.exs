defmodule WordWolfWeb.SpectatorFlowTest do
  use WordWolfWeb.ChannelCase, async: false

  alias WordWolf.{Chat, Repo, Rooms}
  alias WordWolf.Games.{GamePlayer, GameServer}
  alias WordWolf.Rooms.RoomMember
  alias WordWolfWeb.{RoomChannel, UserSocket}

  setup do
    on_exit(&stop_game_servers/0)
    players = Enum.map(1..4, fn _ -> user_fixture() end)
    room = room_fixture(hd(players), players)
    %{players: players, room: room}
  end

  defp connect_member(user, room) do
    socket(UserSocket, "socket:#{user.id}", %{user_id: user.id})
    |> subscribe_and_join(RoomChannel, "room:#{room.id}")
  end

  defp start_discussion(players, room) do
    sockets =
      Map.new(players, fn user ->
        {:ok, _, socket} = connect_member(user, room)
        {user.id, socket}
      end)

    word_group_fixture()
    assert {:ok, %{gameId: game_id}} = GameServer.start_game(room.id, hd(players).id)
    assert_broadcast "phase.changed", %{phase: "DISCUSSION"}
    {game_id, sockets}
  end

  test "接続中のプレイヤーを観戦へ切り替え、戻ると準備未完了になる", %{players: [_, user | _], room: room} do
    {:ok, _, socket} = connect_member(user, room)
    assert {:ok, %{room: updated}} = Rooms.join_room(user.id, room.id, "SPECTATOR")
    refute Enum.any?(updated.players, &(&1.userId == user.id))
    assert Enum.any?(updated.spectators, &(&1.userId == user.id))
    refute Repo.get_by!(RoomMember, room_id: room.id, user_id: user.id).ready
    ref = push(socket, "room.ready", %{"ready" => true})
    assert_reply ref, :error, %{error: %{code: "NOT_A_PLAYER"}}

    assert {:ok, %{room: updated}} = Rooms.join_room(user.id, room.id, "PLAYER")
    player = Enum.find(updated.players, &(&1.userId == user.id))
    refute player.ready
    assert player.connected
    ref = push(socket, "room.ready", %{"ready" => true})
    assert_reply ref, :ok, %{ready: true}
  end

  test "ホストは最古参プレイヤーへホストを引き継いで観戦できる", %{players: [host, next | _], room: room} do
    Rooms.persist_member_ready(room.id, next.id, false)
    assert {:ok, %{room: updated}} = Rooms.join_room(host.id, room.id, "SPECTATOR")
    assert updated.hostUserId == next.id
    assert Enum.find(updated.players, &(&1.userId == next.id)).ready
    assert Enum.any?(updated.spectators, &(&1.userId == host.id))
    assert Repo.get_by!(RoomMember, room_id: room.id, user_id: next.id).ready
  end

  test "最後のプレイヤーは観戦へ切り替えられない", %{players: [host | _]} do
    room = room_fixture(host, [host])
    assert {:error, :invalid_phase} = Rooms.join_room(host.id, room.id, "SPECTATOR")

    assert Repo.get_by!(RoomMember, room_id: room.id, user_id: host.id).participant_type ==
             "PLAYER"
  end

  test "満員の部屋では観戦できるがプレイヤーへ切り替えられない", %{room: room} do
    room |> Ecto.Changeset.change(max_players: 4) |> Repo.update!()
    watcher = user_fixture()
    assert {:ok, _} = Rooms.join_room(watcher.id, room.id, "SPECTATOR")
    assert {:error, :room_full} = Rooms.join_room(watcher.id, room.id, "PLAYER")

    assert Repo.get_by!(RoomMember, room_id: room.id, user_id: watcher.id).participant_type ==
             "SPECTATOR"
  end

  test "開始後は役割を変更できず、お題とプレイヤー枠を維持する", %{players: [host, user | _] = players, room: room} do
    {game_id, _} = start_discussion(players, room)
    assert {:error, :game_in_progress} = Rooms.join_room(user.id, room.id, "SPECTATOR")
    assert {:error, :game_in_progress} = Rooms.join_room(host.id, room.id, "SPECTATOR")
    assert {:error, :game_in_progress} = Rooms.join_room(user_fixture().id, room.id, "PLAYER")
    assert {:ok, _} = GameServer.assignment(game_id, user.id)

    assert Repo.get_by!(RoomMember, room_id: room.id, user_id: user.id).participant_type ==
             "PLAYER"
  end

  test "途中観戦は公開履歴を読めるが、お題・発言・投票は不可で退出しても停止しない", %{players: [sender | _] = players, room: room} do
    {game_id, sockets} = start_discussion(players, room)

    ref =
      push(sockets[sender.id], "chat.send", %{
        "clientMessageId" => "before-watch",
        "content" => "みんなで使う？"
      })

    assert_reply ref, :ok, %{}
    watcher = user_fixture()

    assert {:ok, %{room: %{status: "IN_GAME"}}} =
             Rooms.join_room(watcher.id, room.id, "SPECTATOR")

    assert {:ok, reply, socket} = connect_member(watcher, room)
    assert reply.publicState.phase == "DISCUSSION"
    assert Chat.member?(game_id, watcher.id)
    assert Enum.any?(Chat.list_messages(game_id), &(&1.content == "みんなで使う？"))
    game_player = Repo.get_by!(GamePlayer, game_id: game_id, user_id: watcher.id)
    assert game_player.participant_type == "SPECTATOR"
    assert game_player.assigned_word == nil
    assert game_player.role == nil
    assert {:error, :not_a_player} = GameServer.assignment(game_id, watcher.id)
    ref = push(socket, "chat.send", %{"clientMessageId" => "watcher", "content" => "見てます"})
    assert_reply ref, :error, %{error: %{code: "NOT_A_PLAYER"}}
    server = sockets[sender.id].assigns.game_server
    state = :sys.get_state(server)
    send(server, {:phase_timeout, state.phase_version})
    assert_broadcast "phase.changed", %{phase: "VOTING"}
    assert {:error, :not_a_player} = GameServer.cast_vote(game_id, watcher.id, sender.id)
    assert {:ok, _} = Rooms.leave_room(watcher.id, room.id)
    state = :sys.get_state(sockets[sender.id].assigns.game_server)
    refute state.paused
  end
end
