defmodule WordWolf.Games.GameServerTest do
  use WordWolf.DataCase, async: false

  import WordWolf.AccountsFixtures
  import WordWolf.GameFixtures

  alias WordWolf.Chat
  alias WordWolf.Games.{Game, GamePlayer, GameServer, GameSupervisor, Vote}
  alias WordWolfWeb.Endpoint

  setup do
    # サンドボックスのオーナーが降りる前に GameServer を畳む（game_fixtures.ex の注意書き）
    on_exit(&stop_game_servers/0)

    players = Enum.map(1..4, fn _ -> user_fixture() end)
    room = room_fixture(hd(players), players)
    word_group_fixture()
    {:ok, server} = GameSupervisor.ensure_started(room.id)

    actors =
      Map.new(players, fn user ->
        connection_id = "conn_#{user.id}"
        assert {:ok, _state} = GameServer.connect(server, user.id, connection_id, self())
        {user.id, {user.id, connection_id}}
      end)

    %{players: players, room: room, server: server, actors: actors}
  end

  test "host can start only after every player is connected and ready", context do
    host = hd(context.players)
    Endpoint.subscribe("user:#{host.id}")

    assert {:ok, %{gameId: game_id}} = GameServer.start_game(context.room.id, host.id)
    assert is_binary(game_id)

    assert_receive %Phoenix.Socket.Broadcast{
      event: "private.state",
      payload: %{gameId: ^game_id, word: word, category: "PLACE"}
    }

    assert word in ["図書館", "本屋"]

    assert {:error, :invalid_phase} =
             GameServer.cast_vote(game_id, host.id, List.last(context.players).id)
  end

  test "stale connection cannot change ready state", context do
    user = hd(context.players)

    assert {:error, :stale_connection} =
             GameServer.set_ready(context.server, {user.id, "old"}, false)
  end

  test "disconnect during an active phase pauses and reconnect resumes", context do
    host = hd(context.players)
    actor = Map.fetch!(context.actors, host.id)
    assert {:ok, _} = GameServer.start_game(context.room.id, host.id)

    GameServer.disconnect(context.server, actor)
    assert :sys.get_state(context.server).paused

    assert {:ok, state} = GameServer.connect(context.server, host.id, "conn_new", self())
    refute state.paused
    assert state.phase == "DEALING"
  end

  test "2人では開始できず、3人揃えば市民2人とウルフ1人で開始できる", context do
    players = Enum.take(context.players, 3)
    host = hd(players)
    room = room_fixture(host, players)
    server = start_supervised!({GameServer, room.id})

    for player <- Enum.take(players, 2),
        do: GameServer.connect(server, player.id, "three_#{player.id}", self())

    assert {:error, :not_ready} = GameServer.start_game(room.id, host.id)
    last = List.last(players)
    assert {:ok, _} = GameServer.connect(server, last.id, "three_#{last.id}", self())
    assert {:ok, _} = GameServer.start_game(room.id, host.id)
    state = :sys.get_state(server)
    assert Enum.frequencies(Map.values(state.roles)) == %{"CITIZEN" => 2, "WORD_WOLF" => 1}
  end

  test "全員接続済みでも2人だけなら開始できない", context do
    players = Enum.take(context.players, 2)
    room = room_fixture(hd(players), players)
    server = start_supervised!({GameServer, room.id})
    for player <- players, do: GameServer.connect(server, player.id, "two_#{player.id}", self())
    assert {:error, :not_ready} = GameServer.start_game(room.id, hd(players).id)
  end

  test "2人切断中は1人だけ戻っても停止し、全員揃ったときだけ再開する", context do
    [first, second | _] = context.players
    Endpoint.subscribe("room:#{context.room.id}")
    assert {:ok, %{gameId: game_id}} = GameServer.start_game(context.room.id, first.id)
    assert_discussion()

    for player <- [first, second],
        do: GameServer.disconnect(context.server, context.actors[player.id])

    paused = :sys.get_state(context.server)
    assert paused.paused
    assert MapSet.size(paused.disconnected) == 2
    assert {:ok, state} = GameServer.connect(context.server, first.id, "return_first", self())
    assert state.paused
    assert state.deadline == nil
    partial = :sys.get_state(context.server)
    assert partial.reconnect_deadline == paused.reconnect_deadline
    assert partial.paused_remaining_ms == paused.paused_remaining_ms
    assert partial.disconnected == MapSet.new([second.id])
    assert Repo.get!(Game, game_id).paused
    refute_receive %Phoenix.Socket.Broadcast{event: "game.resumed"}
    assert {:ok, state} = GameServer.connect(context.server, second.id, "return_second", self())
    refute state.paused
    assert state.deadline
    assert_receive %Phoenix.Socket.Broadcast{event: "game.resumed"}
    refute Repo.get!(Game, game_id).paused
  end

  test "一部だけ再接続しても残りの人が期限に戻らなければ中止する", context do
    [first, second | _] = context.players
    Endpoint.subscribe("room:#{context.room.id}")
    assert {:ok, _} = GameServer.start_game(context.room.id, first.id)
    assert_discussion()

    for player <- [first, second],
        do: GameServer.disconnect(context.server, context.actors[player.id])

    assert {:ok, state} = GameServer.connect(context.server, first.id, "return_first", self())
    assert state.paused
    send(context.server, {:reconnect_timeout, state.phaseVersion})
    assert_receive %Phoenix.Socket.Broadcast{event: "game.aborted"}
    assert :sys.get_state(context.server).phase == "LOBBY"
  end

  describe "永続化" do
    test "start_game が games と game_players を書く", context do
      host = hd(context.players)
      assert {:ok, %{gameId: game_id}} = GameServer.start_game(context.room.id, host.id)

      game = Repo.get!(Game, game_id)
      assert game.room_id == context.room.id
      assert game.phase == "DEALING"
      assert game.paused == false
      assert game.started_at
      refute game.ended_at

      rows = Repo.all(from(gp in GamePlayer, where: gp.game_id == ^game_id))
      assert length(rows) == 4
      # DB制約 game_players_assignment_check: PLAYER は role と assigned_word が必須
      assert Enum.all?(rows, &(&1.participant_type == "PLAYER"))
      assert Enum.count(rows, &(&1.role == "WORD_WOLF")) == 1
      assert Enum.all?(rows, &(&1.assigned_word in ["図書館", "本屋"]))
    end

    test "start_gameでプレイヤー全員がIN_GAME（フレンドのオンライン状態）になる", context do
      host = hd(context.players)
      Endpoint.subscribe("room:#{context.room.id}")

      assert {:ok, _} = GameServer.start_game(context.room.id, host.id)

      # WordWolf.Presence.game_started/1 は send(self(), ...) 経由の非同期処理
      # （backend/lib/word_wolf/games/game_server.ex）。DISCUSSION への遷移を待てば、
      # それより先にキューされているのでメールボックスの順序的に反映は完了している
      assert_discussion()

      assert Enum.all?(context.players, fn player ->
               WordWolf.Accounts.get_user(player.id).presence_state == "IN_GAME"
             end)
    end

    test "フェーズ遷移が games.phase に反映され、チャットが保存される", context do
      host = hd(context.players)
      actor = Map.fetch!(context.actors, host.id)
      Endpoint.subscribe("room:#{context.room.id}")

      {:ok, %{gameId: game_id}} = GameServer.start_game(context.room.id, host.id)
      assert_discussion()
      assert Repo.get!(Game, game_id).phase == "DISCUSSION"

      assert {:ok, %{messageId: message_id}} =
               GameServer.send_chat(context.server, actor, "c-1", "  やあ  ")

      # 保存はレーンDの WordWolf.Chat が担う（trim 済みで入る）
      assert [%{messageId: ^message_id, content: "やあ", userId: user_id}] =
               Chat.list_messages(game_id)

      assert user_id == host.id
    end

    test "投票と結果が votes / games に残る", %{players: players} do
      # DISCUSSION を最短にして VOTING まで進める
      # word pair は setup で作成済み（重複すると word_groups_unique_pair_index に当たる）
      room = room_fixture(hd(players), players, %{discussion_ms: 10, vote_ms: 10_000})
      {:ok, server} = GameSupervisor.ensure_started(room.id)

      Enum.each(players, fn user ->
        assert {:ok, _} = GameServer.connect(server, user.id, "conn_#{user.id}", self())
      end)

      Endpoint.subscribe("room:#{room.id}")
      {:ok, %{gameId: game_id}} = GameServer.start_game(room.id, hd(players).id)
      assert_phase("VOTING")

      [voter | rest] = players
      target = List.last(players)
      assert {:ok, _progress} = GameServer.cast_vote(game_id, voter.id, target.id)

      assert [vote] = Repo.all(from(v in Vote, where: v.game_id == ^game_id))
      assert vote.voter_user_id == voter.id
      assert vote.target_user_id == target.id

      # 残り3人が投じると決着する。自己投票は valid_target が弾くので、
      # target 本人だけは voter へ投じさせる。
      Enum.each(rest, fn player ->
        to = if player.id == target.id, do: voter.id, else: target.id
        assert {:ok, _} = GameServer.cast_vote(game_id, player.id, to)
      end)

      assert_phase("RESULT")
      game = Repo.get!(Game, game_id)
      assert game.phase == "RESULT"
      assert game.winner in ["CITIZEN_SIDE", "WOLF_SIDE"]
      assert game.result_reason in ["WOLF_IDENTIFIED", "WOLF_ESCAPED"]
      assert game.ended_at
    end
  end

  describe "ホストは常に準備完了" do
    test "結果のあと LOBBY に戻ると、ホストだけ準備完了のまま残る", %{players: players} do
      room = room_fixture(hd(players), players, %{discussion_ms: 10, vote_ms: 10_000})
      {:ok, server} = GameSupervisor.ensure_started(room.id)

      Enum.each(players, fn user ->
        assert {:ok, _} = GameServer.connect(server, user.id, "conn_#{user.id}", self())
      end)

      Endpoint.subscribe("room:#{room.id}")
      {:ok, %{gameId: game_id}} = GameServer.start_game(room.id, hd(players).id)
      assert_phase("VOTING")

      [voter | rest] = players
      target = List.last(players)
      assert {:ok, _} = GameServer.cast_vote(game_id, voter.id, target.id)

      Enum.each(rest, fn player ->
        to = if player.id == target.id, do: voter.id, else: target.id
        assert {:ok, _} = GameServer.cast_vote(game_id, player.id, to)
      end)

      assert_phase("RESULT")
      {:ok, result_room} = WordWolf.Rooms.get_room(hd(players).id, room.id)
      assert Enum.find(result_room.players, &(&1.userId == hd(players).id)).ready
      # test.exs の result_ms: 50 で LOBBY に戻る
      assert_receive %Phoenix.Socket.Broadcast{
                       event: "room.state",
                       payload: %{phase: "LOBBY", players: lobby_players}
                     },
                     1_000

      host_id = hd(players).id
      assert Enum.find(lobby_players, &(&1.userId == host_id)).ready
      refute Enum.any?(lobby_players, &(&1.userId != host_id and &1.ready))
      {:ok, persisted_room} = WordWolf.Rooms.get_room(host_id, room.id)
      assert persisted_room.status == "WAITING"
      assert persisted_room.currentGameId == nil
      assert Enum.find(persisted_room.players, &(&1.userId == host_id)).ready
      refute Enum.any?(persisted_room.players, &(&1.userId != host_id and &1.ready))

      Enum.each(tl(players), fn player ->
        assert {:ok, _} = GameServer.set_ready(server, {player.id, "conn_#{player.id}"}, true)
      end)

      assert {:ok, %{gameId: next_game_id}} = GameServer.start_game(room.id, host_id)
      assert next_game_id != game_id
    end

    test "ホストが準備を取り消そうとしても準備完了のまま", context do
      host = hd(context.players)

      assert {:ok, %{ready: true}} =
               GameServer.set_ready(context.server, context.actors[host.id], false)
    end

    test "ホストが抜けて移譲されたら、新しいホストは準備完了になる", context do
      [host, next_host | _] = context.players
      assert {:ok, _} = GameServer.set_ready(context.server, context.actors[next_host.id], false)
      Endpoint.subscribe("room:#{context.room.id}")

      assert {:ok, :left} = WordWolf.Rooms.leave_room(host.id, context.room.id)

      next_host_id = next_host.id

      assert_receive %Phoenix.Socket.Broadcast{
                       event: "room.state",
                       payload: %{hostUserId: ^next_host_id, players: lobby_players}
                     },
                     1_000

      assert Enum.find(lobby_players, &(&1.userId == next_host_id)).ready
    end
  end

  defp assert_discussion, do: assert_phase("DISCUSSION")

  defp assert_phase(phase) do
    assert_receive %Phoenix.Socket.Broadcast{
                     event: "phase.changed",
                     payload: %{phase: ^phase}
                   },
                   1_000
  end
end
