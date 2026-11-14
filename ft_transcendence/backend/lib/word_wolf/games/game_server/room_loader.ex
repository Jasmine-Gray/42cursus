defmodule WordWolf.Games.GameServer.RoomLoader do
  @moduledoc false

  import Ecto.Query, warn: false

  alias WordWolf.Games.GameServer.State
  alias WordWolf.Repo
  alias WordWolf.Rooms.{Room, RoomMember, WordGroup}

  @spec load(String.t()) :: {:ok, State.t()} | {:error, :not_found}
  def load(room_id) do
    case Repo.get(Room, room_id) do
      nil -> {:error, :not_found}
      room -> {:ok, room |> Repo.preload(members: [:user]) |> build_state()}
    end
  end

  @spec refresh_member(State.t(), String.t()) :: {:ok, State.t()} | {:error, :forbidden}
  def refresh_member(state, user_id) do
    member = Repo.get_by(RoomMember, room_id: state.room_id, user_id: user_id)
    if member, do: add_member(state, Repo.preload(member, :user)), else: {:error, :forbidden}
  end

  @doc """
  部屋のメンバーをDBから読み直す。RESTでの参加・退出（レーンCの WordWolf.Rooms）を
  取り込むために使う。接続状態はGenServer側が正本なので、既知のユーザーの `connected` は
  いまの値を引き継ぎ、新しく現れたユーザーは未接続として入れる。
  """
  @spec reload_members(State.t()) :: State.t()
  def reload_members(state) do
    members =
      from(m in RoomMember, where: m.room_id == ^state.room_id)
      |> Repo.all()
      |> Repo.preload(:user)

    room = Repo.get(Room, state.room_id)
    host_user_id = if room, do: room.host_user_id, else: state.host_user_id

    settings =
      if state.phase == "LOBBY" and room do
        %{
          state.settings
          | genres: room.genres,
            comeback_enabled: room.comeback_enabled,
            discussion_ms: room.discussion_ms,
            vote_ms: room.vote_ms
        }
      else
        state.settings
      end

    Enum.reduce(
      members,
      %{state | host_user_id: host_user_id, settings: settings, players: %{}, spectators: %{}},
      fn member, acc ->
        connected = connected?(state, member.user_id)
        profile = profile(member, connected, host_user_id)

        if member.participant_type == "PLAYER",
          do: %{acc | players: Map.put(acc.players, member.user_id, profile)},
          else: %{acc | spectators: Map.put(acc.spectators, member.user_id, profile)}
      end
    )
  end

  defp connected?(state, user_id) do
    case Map.get(state.players, user_id) || Map.get(state.spectators, user_id) do
      %{connected: connected} -> connected
      _ -> false
    end
  end

  @spec available_word_groups(State.t()) :: [struct()]
  def available_word_groups(state) do
    previous_group_id =
      Repo.one(
        from(g in WordWolf.Games.Game,
          where: g.room_id == ^state.room_id,
          order_by: [desc: g.started_at, desc: g.id],
          limit: 1,
          select: g.word_group_id
        )
      )

    groups =
      from(w in WordGroup,
        where: w.active == true and w.genre in ^state.settings.genres
      )
      |> Repo.all()

    alternatives = Enum.reject(groups, &(&1.id == previous_group_id))
    if alternatives == [], do: groups, else: alternatives
  end

  defp build_state(room) do
    {players, spectators} =
      Enum.reduce(room.members, {%{}, %{}}, fn member, {players, spectators} ->
        profile = profile(member, false, room.host_user_id)

        if member.participant_type == "PLAYER",
          do: {Map.put(players, member.user_id, profile), spectators},
          else: {players, Map.put(spectators, member.user_id, profile)}
      end)

    %State{
      room_id: room.id,
      host_user_id: room.host_user_id,
      players: players,
      spectators: spectators,
      settings: %{
        genres: room.genres,
        comeback_enabled: room.comeback_enabled,
        discussion_ms: room.discussion_ms,
        vote_ms: room.vote_ms,
        reconnect_grace_ms: room.reconnect_grace_ms,
        min_players: room.min_players,
        max_players: room.max_players
      }
    }
  end

  defp add_member(state, member) do
    cond do
      Map.has_key?(state.players, member.user_id) or
          Map.has_key?(state.spectators, member.user_id) ->
        {:ok, state}

      member.participant_type == "SPECTATOR" ->
        spectators =
          Map.put(state.spectators, member.user_id, profile(member, true, state.host_user_id))

        {:ok, %{state | spectators: spectators}}

      state.phase == "LOBBY" ->
        players =
          Map.put(state.players, member.user_id, profile(member, true, state.host_user_id))

        {:ok, %{state | players: players}}

      true ->
        {:error, :forbidden}
    end
  end

  # ホストは常に準備完了（契約 asyncapi.yaml onRoomReady）。移譲で新しくホストになった人も含む
  defp profile(member, connected, host_user_id) do
    %{
      user_id: member.user_id,
      display_name: member.user.display_name,
      avatar_url: member.user.avatar_url,
      ready: member.ready or member.user_id == host_user_id,
      connected: connected
    }
  end
end
