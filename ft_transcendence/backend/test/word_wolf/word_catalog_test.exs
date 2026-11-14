defmodule WordWolf.WordCatalogTest do
  use WordWolf.DataCase, async: false
  import Ecto.Query
  alias WordWolf.DevSeeds
  alias WordWolf.Games.{Game, GamePlayer}
  alias WordWolf.Games.GameServer.{RoomLoader, State}
  alias WordWolf.Rooms.{Room, WordGroup}

  test "72組のお題を繰り返し投入しても増殖しない" do
    catalog =
      Application.app_dir(:word_wolf, "priv/word-groups.json") |> File.read!() |> Jason.decode!()

    assert length(catalog) == 72
    assert length(Enum.uniq_by(catalog, & &1["id"])) == length(catalog)
    assert length(Enum.uniq_by(catalog, &Enum.sort(&1["words"]))) == length(catalog)

    assert Enum.all?(catalog, fn group ->
             length(group["words"]) == 2 and Enum.uniq(group["words"]) == group["words"] and
               Enum.all?(group["words"], &(is_binary(&1) and String.trim(&1) != ""))
           end)

    DevSeeds.run()
    first = Repo.aggregate(WordGroup, :count)
    DevSeeds.run()
    assert Repo.aggregate(WordGroup, :count) == first

    active_ids = Repo.all(from(w in WordGroup, where: w.active, select: w.id))
    assert Enum.sort(active_ids) == catalog |> Enum.map(& &1["id"]) |> Enum.sort()
  end

  test "現行リストから外したお題は出題を止め、過去のゲームと配布済みのお題を保持する" do
    DevSeeds.run()
    historical_group = Repo.get!(WordGroup, "word_place_library")
    historical_game = Repo.get!(Game, "game_history_001")
    assignments = Repo.all(from(p in GamePlayer, order_by: p.id))

    Repo.insert!(%WordGroup{
      id: "removed_product_group",
      genre: "ROMANCE",
      words: ["旧商品A", "旧商品B"],
      active: true
    })

    historical_group |> Ecto.Changeset.change(active: true) |> Repo.update!()
    room = Repo.get!(Room, "room_case_042")
    room |> Ecto.Changeset.change(room_name: "プレイ中の部屋") |> Repo.update!()

    assert {:ok, _} = DevSeeds.sync_word_groups()
    assert {:ok, _} = DevSeeds.sync_word_groups()

    retired = Repo.get!(WordGroup, "removed_product_group")
    refute retired.active
    assert retired.words == ["旧商品A", "旧商品B"]
    refute Repo.get!(WordGroup, historical_group.id).active
    assert Repo.get!(WordGroup, historical_group.id).words == historical_group.words
    assert Repo.get!(Game, historical_game.id) == historical_game
    assert Repo.all(from(p in GamePlayer, order_by: p.id)) == assignments
    assert Repo.get!(Room, room.id).room_name == "プレイ中の部屋"
  end

  test "廃止ジャンルを設定変更でも保存できない" do
    DevSeeds.run()
    room = Repo.get!(Room, "room_case_042")

    for genre <- ~w(ROMANCE VALUES) do
      assert {:error, :invalid_payload} =
               WordWolf.Rooms.persist_settings(room.host_user_id, room.id, %{"genres" => [genre]})

      assert Repo.get!(Room, room.id).genres == room.genres
    end
  end

  test "各ジャンルで現行のお題だけを抽選できる" do
    DevSeeds.run()

    for genre <- ~w(PLACE FOOD HOBBY DAILY_ITEM WORK_SCHOOL ENTERTAINMENT) do
      state = %State{room_id: "room_case_042", settings: %{genres: [genre]}}
      groups = RoomLoader.available_word_groups(state)

      assert length(groups) == 12
      assert Enum.all?(groups, &(&1.active and &1.genre == genre))
    end

    state = %State{room_id: "room_case_042", settings: %{genres: ["ROMANCE", "VALUES"]}}
    assert length(RoomLoader.available_word_groups(state)) == 0
  end
end
