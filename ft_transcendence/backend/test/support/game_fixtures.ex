defmodule WordWolf.GameFixtures do
  @moduledoc false

  alias WordWolf.Repo
  alias WordWolf.Rooms.{Room, RoomMember, WordGroup}

  def room_fixture(host, members, attrs \\ %{}) do
    suffix = System.unique_integer([:positive])

    room =
      %Room{}
      |> Ecto.Changeset.change(
        Map.merge(
          %{
            id: "room_test_#{suffix}",
            host_user_id: host.id,
            room_name: "テストルーム",
            invite_code: "CODE#{suffix}",
            genres: ["PLACE"],
            discussion_ms: 60_000,
            vote_ms: 60_000,
            reconnect_grace_ms: 20_000,
            min_players: 3,
            max_players: 8
          },
          attrs
        )
      )
      |> Repo.insert!()

    members
    |> Enum.with_index()
    |> Enum.each(fn {user, index} ->
      insert_member(room, user, "PLAYER", "member_test_#{suffix}_#{index}")
    end)

    room
  end

  @doc """
  既存の部屋に観戦者を足す。観戦者は購読できるが chat.send / room.ready はできない
  （契約 _docs/spec/api/asyncapi.yaml onChatSend）。
  """
  def spectator_fixture(room, user) do
    insert_member(room, user, "SPECTATOR", "member_spec_#{System.unique_integer([:positive])}")
    user
  end

  defp insert_member(room, user, participant_type, id) do
    %RoomMember{}
    |> Ecto.Changeset.change(%{
      id: id,
      room_id: room.id,
      user_id: user.id,
      participant_type: participant_type,
      # DB制約 room_members_spectator_not_ready_check: 観戦者は ready = false のみ
      ready: participant_type == "PLAYER",
      connected: true,
      joined_at: DateTime.utc_now()
    })
    |> Repo.insert!()
  end

  @doc """
  起動中の GameServer をすべて畳む。ExUnit の `on_exit` へ渡して、サンドボックスの
  オーナーが降りる前に走らせること（on_exit は登録の逆順に実行される）。
  """
  def stop_game_servers do
    WordWolf.Games.GameSupervisor
    |> DynamicSupervisor.which_children()
    |> Enum.each(fn {_id, pid, _type, _modules} -> if is_pid(pid), do: stop_server(pid) end)
  end

  # terminate_child は実行中のクエリごと即座に落とすため Postgrex が
  # 「client exited」で切断ログを吐く。処理中のメッセージを終えてから畳む。
  defp stop_server(pid) do
    GenServer.stop(pid, :normal, 1_000)
  catch
    :exit, _reason -> :ok
  end

  def word_group_fixture do
    suffix = System.unique_integer([:positive])

    %WordGroup{}
    |> Ecto.Changeset.change(%{
      id: "word_group_test_#{suffix}",
      genre: "PLACE",
      words: ["図書館", "本屋"],
      active: true
    })
    |> Repo.insert!()
  end
end
