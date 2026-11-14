defmodule WordWolfWeb.UserChannelTest do
  use WordWolfWeb.ChannelCase, async: false

  alias WordWolf.Rooms
  alias WordWolfWeb.{RoomChannel, UserChannel, UserSocket}

  setup do
    on_exit(&stop_game_servers/0)

    players = Enum.map(1..4, fn _ -> user_fixture() end)
    room = room_fixture(hd(players), players)
    %{user: hd(players), players: players, room: room}
  end

  describe "部屋の文脈あり（ゲーム中）" do
    test "本人かつ対象roomの最新connectionだけがjoinできる", %{user: user, room: room} do
      socket = socket(UserSocket, "socket:#{user.id}", %{user_id: user.id})

      assert {:ok, room_reply, _room_socket} =
               subscribe_and_join(socket, RoomChannel, "room:#{room.id}")

      params = %{"roomId" => room.id, "connectionId" => room_reply.connectionId}
      user_socket = socket(UserSocket, "user-socket:#{user.id}", %{user_id: user.id})

      assert {:ok, _, _} = subscribe_and_join(user_socket, UserChannel, "user:#{user.id}", params)
    end

    test "古いconnectionIdは拒否する", %{user: user, room: room} do
      socket = socket(UserSocket, "socket:#{user.id}", %{user_id: user.id})
      params = %{"roomId" => room.id, "connectionId" => "conn_old"}

      assert {:error, %{status: "error", error: %{code: "FORBIDDEN"}}} =
               subscribe_and_join(socket, UserChannel, "user:#{user.id}", params)
    end
  end

  describe "部屋の文脈なし（ホーム画面など）" do
    test "パラメータ無しでも本人なら join できる", %{user: user} do
      socket = socket(UserSocket, "socket:#{user.id}", %{user_id: user.id})

      assert {:ok, _reply, joined} = subscribe_and_join(socket, UserChannel, "user:#{user.id}")
      assert joined.assigns.user_id == user.id
    end

    test "他人のtopicには join できない", %{user: user} do
      other = user_fixture()
      socket = socket(UserSocket, "socket:#{user.id}", %{user_id: user.id})

      assert {:error, %{status: "error", error: %{code: "FORBIDDEN"}}} =
               subscribe_and_join(socket, UserChannel, "user:#{other.id}")
    end

    test "部屋に入っていなくても room.invited を受け取れる", %{user: invitee} do
      inviter = user_fixture()
      socket = socket(UserSocket, "socket:#{invitee.id}", %{user_id: invitee.id})
      {:ok, _reply, _joined} = subscribe_and_join(socket, UserChannel, "user:#{invitee.id}")

      {:ok, room} = Rooms.create_room(inviter.id, %{"roomName" => "テスト部屋"})
      assert {:ok, invite} = Rooms.invite_to_room(inviter.id, room.roomId, invitee.id)

      assert_push "room.invited", payload
      assert payload.inviteId == invite.inviteId
      assert payload.roomId == room.roomId
      assert payload.roomName == "テスト部屋"
      assert payload.from.userId == inviter.id
    end
  end

  describe "オンライン状態（WordWolf.Presence）" do
    test "joinでONLINEになり、切断でOFFLINEに戻る。フレンドへ配信される", %{} do
      # user_fixture/1 は presence_state のデフォルトが "ONLINE" なので、
      # ここでは遷移を見たいのでOFFLINEから明示的に始める
      user = user_fixture(%{presence_state: "OFFLINE"})
      friend = user_fixture()
      {:ok, %{requestId: request_id}} = WordWolf.Friends.send_friend_request(user.id, friend.id)
      {:ok, _} = WordWolf.Friends.accept_friend_request(friend.id, request_id)

      friend_socket = socket(UserSocket, "socket:#{friend.id}", %{user_id: friend.id})
      {:ok, _reply, friend_joined} = subscribe_and_join(friend_socket, UserChannel, "user:#{friend.id}")

      socket = socket(UserSocket, "socket:#{user.id}", %{user_id: user.id})
      {:ok, _reply, joined} = subscribe_and_join(socket, UserChannel, "user:#{user.id}")

      assert WordWolf.Accounts.get_user(user.id).presence_state == "ONLINE"

      assert_push "friend.presence_changed", %{userId: user_id, state: "ONLINE"}
      assert user_id == user.id

      # close/leave はデフォルトでテストプロセスを道連れに落とすので、先に unlink する
      # （Phoenix.ChannelTest の "Leave and close" を参照）
      Process.unlink(joined.channel_pid)
      close(joined)

      assert_push "friend.presence_changed", %{userId: ^user_id, state: "OFFLINE"}
      assert WordWolf.Accounts.get_user(user.id).presence_state == "OFFLINE"

      Process.unlink(friend_joined.channel_pid)
      close(friend_joined)
    end
  end
end
