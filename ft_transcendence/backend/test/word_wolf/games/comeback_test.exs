defmodule WordWolf.Games.ComebackTest do
  use WordWolf.DataCase, async: false
  import WordWolf.AccountsFixtures
  import WordWolf.GameFixtures
  alias WordWolf.Games.{Game, GameServer}
  alias WordWolf.Games.GameServer.{Presenter, RoomLoader}
  alias WordWolf.Rooms.{Room, WordGroup}
  alias WordWolfWeb.Endpoint

  setup do
    old_dealing = Application.get_env(:word_wolf, :dealing_ms)
    old_result = Application.get_env(:word_wolf, :result_ms)
    Application.put_env(:word_wolf, :dealing_ms, 60_000)
    Application.put_env(:word_wolf, :result_ms, 60_000)

    on_exit(fn ->
      Application.put_env(:word_wolf, :dealing_ms, old_dealing)
      Application.put_env(:word_wolf, :result_ms, old_result)
    end)

    players = Enum.map(1..4, fn _ -> user_fixture() end)
    room = room_fixture(hd(players), players, %{comeback_enabled: true})
    group = word_group_fixture()
    server = start_supervised!({GameServer, room.id})

    Enum.each(players, fn player ->
      assert {:ok, _} = GameServer.connect(server, player.id, player.id, self())
    end)

    %{players: players, room: room, server: server, group: group}
  end

  defp advance(server) do
    state = :sys.get_state(server)
    send(server, {:phase_timeout, state.phase_version})
    :sys.get_state(server)
  end

  defp identify_wolf(context) do
    assert {:ok, %{gameId: game_id}} =
             GameServer.start_game(context.room.id, hd(context.players).id)

    advance(context.server)
    state = advance(context.server)
    wolf_id = Enum.find_value(state.roles, fn {id, role} -> if role == "WORD_WOLF", do: id end)
    citizen_id = Enum.find_value(state.roles, fn {id, role} -> if role == "CITIZEN", do: id end)

    Enum.each(context.players, fn player ->
      target = if player.id == wolf_id, do: citizen_id, else: wolf_id
      assert {:ok, _} = GameServer.cast_vote(game_id, player.id, target)
    end)

    {game_id, wolf_id, citizen_id}
  end

  test "公開入力に秘密を混ぜず、最新の最終回答で逆転して結果を永続化する", context do
    {game_id, wolf_id, citizen_id} = identify_wolf(context)
    Endpoint.subscribe("room:#{context.room.id}")
    assert {:ok, public} = GameServer.state(context.server, {wolf_id, wolf_id})
    assert public.phase == "COMEBACK"
    assert public.comeback == %{gameId: game_id, wolfUserId: wolf_id, content: ""}
    refute Map.has_key?(public, :words)
    assert {:error, :invalid_phase} = GameServer.game_result(game_id, citizen_id)
    assert {:ok, _} = GameServer.comeback_draft(context.server, {wolf_id, wolf_id}, "入力の途中")

    assert_receive %Phoenix.Socket.Broadcast{
      event: "comeback.updated",
      payload: %{content: "入力の途中"}
    }

    assert {:error, :rate_limited} =
             GameServer.comeback_draft(context.server, {wolf_id, wolf_id}, "速すぎる")

    state = :sys.get_state(context.server)
    answer = Presenter.majority_word(state)

    assert {:ok, _} =
             GameServer.comeback_submit(context.server, {wolf_id, wolf_id}, "　#{answer} ")

    assert {:ok, result} = GameServer.game_result(game_id, citizen_id)
    assert result.winner == "WOLF_SIDE"
    assert result.reason == "WOLF_GUESSED"
    assert result.comeback == %{answer: answer, correct: true, timedOut: false}
    assert Repo.get!(Game, game_id).comeback_result["correct"]

    assert {:error, :invalid_phase} =
             GameServer.comeback_submit(context.server, {wolf_id, wolf_id}, answer)
  end

  test "市民・観戦者・古い接続は回答できず、空や長すぎる回答を拒否する", context do
    {_game_id, wolf_id, citizen_id} = identify_wolf(context)

    assert {:error, :forbidden} =
             GameServer.comeback_draft(context.server, {citizen_id, citizen_id}, "盗み見")

    spectator = user_fixture()
    spectator_fixture(context.room, spectator)
    assert {:ok, _} = GameServer.connect(context.server, spectator.id, "spectator", self())

    assert {:error, :forbidden} =
             GameServer.comeback_submit(context.server, {spectator.id, "spectator"}, "答え")

    assert {:error, :stale_connection} =
             GameServer.comeback_submit(context.server, {wolf_id, "old"}, "答え")

    assert {:error, :invalid_payload} =
             GameServer.comeback_submit(context.server, {wolf_id, wolf_id}, "　 ")

    assert {:error, :invalid_payload} =
             GameServer.comeback_draft(
               context.server,
               {wolf_id, wolf_id},
               String.duplicate("字", 81)
             )
  end

  test "未確定の正しい下書きも時間切れなら市民勝利", context do
    {game_id, wolf_id, citizen_id} = identify_wolf(context)
    answer = Presenter.majority_word(:sys.get_state(context.server))
    assert {:ok, _} = GameServer.comeback_draft(context.server, {wolf_id, wolf_id}, answer)
    advance(context.server)
    assert {:ok, result} = GameServer.game_result(game_id, citizen_id)
    assert result.winner == "CITIZEN_SIDE"
    assert result.comeback == %{answer: answer, correct: false, timedOut: true}
  end

  test "不正解は市民勝利", context do
    {game_id, wolf_id, citizen_id} = identify_wolf(context)
    assert {:ok, _} = GameServer.comeback_submit(context.server, {wolf_id, wolf_id}, "絶対に別のお題")
    assert {:ok, result} = GameServer.game_result(game_id, citizen_id)
    assert result.winner == "CITIZEN_SIDE"
    refute result.comeback.correct
  end

  test "接続切断では一時停止し、再接続で下書きと残り時間を復元する", context do
    {_game_id, wolf_id, citizen_id} = identify_wolf(context)
    assert {:ok, _} = GameServer.comeback_draft(context.server, {wolf_id, wolf_id}, "書きかけ")
    GameServer.disconnect(context.server, {wolf_id, wolf_id})
    paused = :sys.get_state(context.server)
    assert paused.paused

    assert {:error, :game_paused} =
             GameServer.comeback_draft(context.server, {citizen_id, citizen_id}, "中断")

    assert {:ok, resumed} = GameServer.connect(context.server, wolf_id, "new", self())
    refute resumed.paused
    assert resumed.comeback.content == "書きかけ"

    assert DateTime.diff(
             DateTime.from_iso8601(resumed.deadline) |> elem(1),
             DateTime.utc_now(),
             :millisecond
           ) <= 30_000
  end

  test "部屋設定OFFでは従来どおり投票だけで確定する", context do
    context.room |> Ecto.Changeset.change(comeback_enabled: false) |> Repo.update!()
    GameServer.refresh(context.server)
    _ = :sys.get_state(context.server)
    {game_id, _wolf_id, citizen_id} = identify_wolf(context)
    assert {:ok, result} = GameServer.game_result(game_id, citizen_id)
    assert result.winner == "CITIZEN_SIDE"
    assert result.comeback == nil
  end

  test "同票では逆転回答へ移らずウルフが逃げ切る", context do
    {:ok, %{gameId: game_id}} = GameServer.start_game(context.room.id, hd(context.players).id)
    advance(context.server)
    advance(context.server)
    targets = tl(context.players) ++ [hd(context.players)]

    Enum.zip(context.players, targets)
    |> Enum.each(fn {player, target} -> GameServer.cast_vote(game_id, player.id, target.id) end)

    assert {:ok, result} = GameServer.game_result(game_id, hd(context.players).id)
    assert result.reason == "WOLF_ESCAPED"
    assert result.comeback == nil
  end

  test "3語グループから異なる2語を選び、次回は別グループを候補にする", context do
    context.group |> Ecto.Changeset.change(words: ["パルム", "HERSHEY'S", "パキシエル"]) |> Repo.update!()
    {:ok, %{gameId: game_id}} = GameServer.start_game(context.room.id, hd(context.players).id)
    state = :sys.get_state(context.server)
    assert length(Enum.uniq(Map.values(state.words))) == 2
    assert Enum.all?(Map.values(state.words), &(&1 in ["パルム", "HERSHEY'S", "パキシエル"]))
    assert Repo.get!(Game, game_id).word_group_id == context.group.id

    other =
      %WordGroup{id: "another_group", genre: "PLACE", words: ["犬", "猫"], active: true}
      |> Repo.insert!()

    assert Enum.map(RoomLoader.available_word_groups(state), & &1.id) == [other.id]
    Repo.delete!(other)
    assert Enum.map(RoomLoader.available_word_groups(state), & &1.id) == [context.group.id]
  end

  test "締切後の回答はタイマー通知が届く前でも拒否する", context do
    {_game_id, wolf_id, _citizen_id} = identify_wolf(context)

    :sys.replace_state(context.server, fn state ->
      %{state | deadline: DateTime.add(DateTime.utc_now(), -1)}
    end)

    assert {:error, :deadline_exceeded} =
             GameServer.comeback_submit(context.server, {wolf_id, wolf_id}, "答え")
  end

  test "全角英字・空白・かなの表記差を正規化して判定する", context do
    context.group |> Ecto.Changeset.change(words: ["カフェＡ", "ケーキＢ"]) |> Repo.update!()
    {game_id, wolf_id, citizen_id} = identify_wolf(context)

    answer =
      Presenter.majority_word(:sys.get_state(context.server))
      |> String.normalize(:nfkc)
      |> String.downcase()
      |> String.to_charlist()
      |> Enum.map(fn c -> if c in 0x30A1..0x30F6, do: c - 0x60, else: c end)
      |> List.to_string()

    assert {:ok, _} =
             GameServer.comeback_submit(context.server, {wolf_id, wolf_id}, "　#{answer} ")

    assert {:ok, %{reason: "WOLF_GUESSED"}} = GameServer.game_result(game_id, citizen_id)
  end

  test "部屋の逆転設定は保存され、難易度は公開しない", context do
    host = hd(context.players)

    assert {:ok, room} =
             WordWolf.Rooms.update_settings(host.id, context.room.id, %{
               "comebackEnabled" => false
             })

    refute room.settings.comebackEnabled
    refute Map.has_key?(room.settings, :wordProximity)
    refute Repo.get!(Room, context.room.id).comeback_enabled

    assert {:error, :invalid_payload} =
             WordWolf.Rooms.update_settings(host.id, context.room.id, %{
               "comebackEnabled" => "yes"
             })
  end
end
