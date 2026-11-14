defmodule WordWolf.PresenceTest do
  use WordWolf.DataCase, async: true

  import WordWolf.AccountsFixtures
  import WordWolf.GameFixtures

  alias WordWolf.{Accounts, Friends, Games, Presence}
  alias WordWolf.Accounts.User

  setup do
    a = user_fixture(%{presence_state: "OFFLINE"})
    b = user_fixture(%{presence_state: "OFFLINE"})
    stranger = user_fixture(%{presence_state: "OFFLINE"})

    {:ok, %{requestId: request_id}} = Friends.send_friend_request(a.id, b.id)
    {:ok, _friend} = Friends.accept_friend_request(b.id, request_id)

    Phoenix.PubSub.subscribe(WordWolf.PubSub, "user:#{b.id}")
    Phoenix.PubSub.subscribe(WordWolf.PubSub, "user:#{stranger.id}")

    %{a: a, b: b, stranger: stranger}
  end

  describe "connect/1, disconnect/1（ONLINE/OFFLINE）" do
    test "最初の接続でONLINEになり、ACCEPTEDなフレンドにだけ配信される", %{a: a, b: b, stranger: stranger} do
      b_topic = "user:#{b.id}"
      stranger_topic = "user:#{stranger.id}"

      assert :ok = Presence.connect(a.id)

      assert %User{presence_state: "ONLINE"} = Accounts.get_user(a.id)

      assert_receive %Phoenix.Socket.Broadcast{
        topic: ^b_topic,
        event: "friend.presence_changed",
        payload: %{userId: user_id, state: "ONLINE"}
      }

      assert user_id == a.id
      # フレンドではない stranger のトピックへは配信されない
      refute_received %Phoenix.Socket.Broadcast{topic: ^stranger_topic}
    end

    test "2本目の接続では状態が変わらないので再配信しない", %{a: a} do
      Presence.connect(a.id)
      assert_receive %Phoenix.Socket.Broadcast{event: "friend.presence_changed"}

      Presence.connect(a.id)
      refute_receive %Phoenix.Socket.Broadcast{event: "friend.presence_changed"}
    end

    test "最後の接続が切れたらOFFLINEになりlast_seen_atが入る", %{a: a} do
      Presence.connect(a.id)
      Presence.connect(a.id)
      assert_receive %Phoenix.Socket.Broadcast{event: "friend.presence_changed"}

      # 1本切れただけではまだ ONLINE のまま（複数タブ）
      Presence.disconnect(a.id)
      refute_receive %Phoenix.Socket.Broadcast{event: "friend.presence_changed"}
      assert %User{presence_state: "ONLINE"} = Accounts.get_user(a.id)

      Presence.disconnect(a.id)
      assert_receive %Phoenix.Socket.Broadcast{
        event: "friend.presence_changed",
        payload: %{state: "OFFLINE"}
      }

      assert %User{presence_state: "OFFLINE", last_seen_at: %DateTime{}} = Accounts.get_user(a.id)
    end

    test "disconnectを余分に呼んでも接続数は0未満にならない", %{a: a} do
      Presence.disconnect(a.id)
      Presence.disconnect(a.id)

      assert %User{ws_connection_count: 0} = Accounts.get_user(a.id)
    end
  end

  describe "game_started/1, game_ended/1（IN_GAME）" do
    # game_started/1・game_ended/1 は GameServer が持つ player_id 一覧をそのまま渡す
    # 前提の関数（DBは in_active_game?/1 の判定にしか使わない）。呼び出し元
    # backend/lib/word_wolf/games/game_server.ex の start_game / reset_lobby を参照。

    test "ゲーム開始でPLAYERはIN_GAME、SPECTATORは呼ばれても変わらない", %{a: a, b: b} do
      host = user_fixture()
      room = room_fixture(host, [a, host])
      spectator_fixture(room, b)
      group = word_group_fixture()

      {:ok, game} =
        Games.create_game(%{
          game_id: "game_test_#{System.unique_integer([:positive])}",
          room_id: room.id,
          word_group_id: group.id,
          phase: "DEALING",
          phase_version: 1,
          deadline: nil,
          roles: %{a.id => "CITIZEN", host.id => "WORD_WOLF"},
          words: %{a.id => "図書館", host.id => "本屋"},
          spectator_ids: [b.id]
        })

      # 観戦者は participant_type が PLAYER ではないので、そもそも player_ids に含めない
      # （game_server.ex は Map.keys(state.players) しか渡さない）
      Presence.game_started([a.id, host.id])

      assert_receive %Phoenix.Socket.Broadcast{
        event: "friend.presence_changed",
        payload: %{userId: user_id, state: "IN_GAME"}
      }

      assert user_id == a.id
      assert %User{presence_state: "IN_GAME"} = Accounts.get_user(a.id)
      # 観戦者（b）はプレイヤーではないので IN_GAME にならない
      assert %User{presence_state: "OFFLINE"} = Accounts.get_user(b.id)

      # 実際の順序を再現: ended_at はRESULT遷移（Games.update_phase）で先に立つ。
      # そのあとでゲームサーバーのreset_lobbyがgame_ended/1を呼ぶ（backend/lib/word_wolf/games/game_server.ex）
      Games.update_phase(game.id, "RESULT", 2, nil)
      Presence.game_ended([a.id, host.id])

      assert_receive %Phoenix.Socket.Broadcast{
        event: "friend.presence_changed",
        payload: %{userId: ^user_id, state: "OFFLINE"}
      }

      assert %User{presence_state: "OFFLINE"} = Accounts.get_user(a.id)
    end

    test "接続中にゲームが終わるとONLINEへ戻る", %{a: a} do
      host = user_fixture()
      room = room_fixture(host, [a, host])
      group = word_group_fixture()
      Presence.connect(a.id)
      assert_receive %Phoenix.Socket.Broadcast{event: "friend.presence_changed"}

      {:ok, game} =
        Games.create_game(%{
          game_id: "game_test_#{System.unique_integer([:positive])}",
          room_id: room.id,
          word_group_id: group.id,
          phase: "DEALING",
          phase_version: 1,
          deadline: nil,
          roles: %{a.id => "CITIZEN", host.id => "WORD_WOLF"},
          words: %{a.id => "図書館", host.id => "本屋"},
          spectator_ids: []
        })

      Presence.game_started([a.id, host.id])
      assert_receive %Phoenix.Socket.Broadcast{payload: %{state: "IN_GAME"}}

      # 実際の順序を再現: aborted_at はGames.abort_gameで先に立つ。そのあと
      # reset_lobbyがgame_ended/1を呼ぶ（backend/lib/word_wolf/games/game_server.ex）
      Games.abort_game(game.id)
      Presence.game_ended([a.id, host.id])

      assert_receive %Phoenix.Socket.Broadcast{payload: %{state: "ONLINE"}}
      assert %User{presence_state: "ONLINE"} = Accounts.get_user(a.id)
    end
  end

  describe "resolve_state/1" do
    test "IN_GAME が ONLINE より優先される", %{a: a} do
      host = user_fixture()
      room = room_fixture(host, [a, host])
      group = word_group_fixture()
      Presence.connect(a.id)

      {:ok, _game} =
        Games.create_game(%{
          game_id: "game_test_#{System.unique_integer([:positive])}",
          room_id: room.id,
          word_group_id: group.id,
          phase: "DEALING",
          phase_version: 1,
          deadline: nil,
          roles: %{a.id => "CITIZEN", host.id => "WORD_WOLF"},
          words: %{a.id => "図書館", host.id => "本屋"},
          spectator_ids: []
        })

      assert Presence.resolve_state(a.id) == "IN_GAME"
    end
  end
end
