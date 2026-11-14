defmodule WordWolf.Rooms do
  @moduledoc false

  import Ecto.Query, warn: false
  alias WordWolf.Accounts
  alias WordWolf.Accounts.User
  alias WordWolf.Repo
  alias WordWolf.Rooms.{Room, RoomInvite, RoomMember}

  @doc """
  ユーザーが「参加中」の部屋を RoomPreview 形で返す。無ければ nil。
  """
  @spec active_preview_for(String.t()) :: map() | nil
  def active_preview_for(user_id) do
    room =
      from(r in Room,
        join: m in RoomMember,
        on: m.room_id == r.id,
        where: m.user_id == ^user_id and r.status != "CLOSED",
        order_by: [desc: m.joined_at],
        limit: 1,
        preload: [:members]
      )
      |> Repo.one()

    if room, do: preview(room), else: nil
  end

  @doc """
  招待コードから有効な部屋の概要（RoomPreview）を取得する。
  """
  @spec get_preview_by_code(String.t()) :: map() | nil
  def get_preview_by_code(invite_code) when is_binary(invite_code) do
    room =
      from(r in Room,
        where: r.invite_code == ^invite_code and r.status != "CLOSED",
        preload: [:members]
      )
      |> Repo.one()

    if room, do: preview(room), else: nil
  end

  @doc """
  部屋IDから部屋情報を取得する（RoomState）。
  """
  @spec get_room(String.t(), String.t()) :: {:ok, map()} | {:error, :not_found | :forbidden}
  def get_room(user_id, room_id) when is_binary(user_id) and is_binary(room_id) do
    room =
      Repo.get(Room, room_id)
      |> Repo.preload([:members, members: :user])

    case room do
      nil ->
        {:error, :not_found}

      %Room{status: "CLOSED"} ->
        {:error, :not_found}

      %Room{} = r ->
        is_member = Enum.any?(r.members, &(&1.user_id == user_id))

        if is_member do
          {:ok, to_room_state(r)}
        else
          {:error, :forbidden}
        end
    end
  end

  @doc """
  新しい部屋を作成する（POST /rooms）。
  作成者が過去に作成した WAITING 部屋は自動的にクローズし、未処理の招待を破棄する。
  """
  @spec create_room(String.t(), map()) :: {:ok, map()} | {:error, :invalid_payload}
  def create_room(host_user_id, params) when is_binary(host_user_id) and is_map(params) do
    room_name = Map.get(params, "roomName")
    genres = Map.get(params, "genres", ["PLACE", "FOOD", "HOBBY"])
    comeback_enabled = Map.get(params, "comebackEnabled", false)
    discussion_ms = Map.get(params, "discussionMs", 300_000)
    vote_ms = Map.get(params, "voteMs", 30_000)

    if is_binary(room_name) and String.length(String.trim(room_name)) >= 2 and
         valid_genres?(genres) and is_boolean(comeback_enabled) and
         valid_timing?(discussion_ms, vote_ms) do
      now = DateTime.utc_now() |> DateTime.truncate(:microsecond)

      # 1. 作成者がホストしている既存の有効な部屋をすべて CLOSED にする
      old_rooms =
        from(r in Room,
          where: r.host_user_id == ^host_user_id and r.status != "CLOSED"
        )
        |> Repo.all()

      for old_room <- old_rooms do
        old_room
        |> Room.changeset(%{status: "CLOSED", closed_at: now})
        |> Repo.update!()

        # 古い部屋宛ての未処理招待を DECLINED に更新
        from(i in RoomInvite,
          where: i.room_id == ^old_room.id and i.status == "PENDING"
        )
        |> Repo.update_all(set: [status: "DECLINED", responded_at: now])
      end

      # 2. 作成者が過去にメンバーとして参加していた部屋の RoomMember も整理
      from(m in RoomMember,
        join: r in Room,
        on: r.id == m.room_id,
        where: m.user_id == ^host_user_id and r.status == "WAITING"
      )
      |> Repo.delete_all()

      room_id = "room_#{Ecto.UUID.generate() |> String.slice(0, 16)}"
      code_num = :rand.uniform(900) + 99
      invite_code = "CASE-#{code_num}"

      room =
        %Room{
          id: room_id,
          room_name: String.trim(room_name),
          invite_code: invite_code,
          status: "WAITING",
          genres: genres,
          comeback_enabled: comeback_enabled,
          host_user_id: host_user_id,
          discussion_ms: discussion_ms,
          vote_ms: vote_ms,
          min_players: 3,
          max_players: 8,
          reconnect_grace_ms: 20_000,
          inserted_at: now,
          updated_at: now
        }
        |> Repo.insert!()

      member_id = "rmem_#{Ecto.UUID.generate() |> String.slice(0, 16)}"

      %RoomMember{
        id: member_id,
        room_id: room_id,
        user_id: host_user_id,
        participant_type: "PLAYER",
        ready: true,
        connected: true,
        joined_at: now
      }
      |> Repo.insert!()

      {:ok, to_room_state(room)}
    else
      {:error, :invalid_payload}
    end
  end

  @doc """
  自分宛ての有効な招待一覧（部屋が WAITING 状態、かつ PENDING 状態）を新しい順に取得する。
  """
  @spec list_my_invites(String.t()) :: [map()]
  def list_my_invites(user_id) do
    from(i in RoomInvite,
      join: r in Room,
      on: r.id == i.room_id,
      where: i.invitee_id == ^user_id and i.status == "PENDING" and r.status == "WAITING",
      order_by: [desc: i.sent_at],
      preload: [:room, :inviter]
    )
    |> Repo.all()
    |> Enum.map(&to_room_invite_map/1)
  end

  @doc """
  部屋への招待をフレンドへ送信する。
  部屋が WAITING 状態でない場合は {:error, :invalid_phase}。
  相手がすでに部屋のメンバーの場合は {:error, :already_joined}。
  すでに招待済みの場合は既存招待を返しつつ、Channelへブロードキャストする。
  """
  @spec invite_to_room(String.t(), String.t(), String.t()) ::
          {:ok, map()}
          | {:error, :not_found | :already_joined | :invalid_phase | :invalid_payload}
  def invite_to_room(inviter_user_id, room_id, target_user_id) do
    with %Room{} = room <- Repo.get(Room, room_id) |> Repo.preload([:members, :host_user]),
         %User{} = _inviter <- Repo.get(User, inviter_user_id),
         %User{} = _invitee <- Repo.get(User, target_user_id) do
      cond do
        room.status != "WAITING" ->
          {:error, :invalid_phase}

        Enum.any?(room.members, &(&1.user_id == target_user_id)) ->
          {:error, :already_joined}

        true ->
          now = DateTime.utc_now() |> DateTime.truncate(:microsecond)
          id = "rinv_#{Ecto.UUID.generate() |> String.slice(0, 16)}"

          # 既存の PENDING な招待があるか確認
          existing =
            from(i in RoomInvite,
              where:
                i.room_id == ^room_id and i.invitee_id == ^target_user_id and
                  i.status == "PENDING"
            )
            |> Repo.one()

          invite_record =
            if existing do
              existing
            else
              %RoomInvite{
                id: id,
                room_id: room_id,
                inviter_id: inviter_user_id,
                invitee_id: target_user_id,
                status: "PENDING",
                sent_at: now
              }
              |> Repo.insert!()
            end
            |> Repo.preload([:room, :inviter])

          payload = to_room_invite_map(invite_record)

          # 相手の本人チャンネルへ PubSub 経由で通知
          WordWolfWeb.Endpoint.broadcast("user:#{target_user_id}", "room.invited", payload)

          {:ok, payload}
      end
    else
      nil -> {:error, :not_found}
      _ -> {:error, :invalid_payload}
    end
  end

  @doc """
  部屋招待を承認して部屋へ参加する。
  部屋が WAITING 状態でない場合は {:error, :not_found}。
  """
  @spec accept_room_invite(String.t(), String.t(), String.t()) ::
          {:ok, %{room: map(), realtime: map()}} | {:error, :not_found | :room_full | :conflict}
  def accept_room_invite(user_id, invite_id, participant_type \\ "PLAYER") do
    invite =
      Repo.get_by(RoomInvite, id: invite_id, invitee_id: user_id)
      |> Repo.preload([:room, :inviter])

    case invite do
      nil ->
        {:error, :not_found}

      %RoomInvite{status: status} when status != "PENDING" ->
        {:error, :not_found}

      %RoomInvite{room: %Room{status: status}} when status != "WAITING" ->
        now = DateTime.utc_now() |> DateTime.truncate(:microsecond)

        invite
        |> RoomInvite.changeset(%{status: "DECLINED", responded_at: now})
        |> Repo.update!()

        {:error, :not_found}

      %RoomInvite{} = inv ->
        now = DateTime.utc_now() |> DateTime.truncate(:microsecond)

        # 部屋に参加
        case join_room(user_id, inv.room_id, participant_type) do
          {:ok, result} ->
            inv
            |> RoomInvite.changeset(%{status: "ACCEPTED", responded_at: now})
            |> Repo.update!()

            {:ok, result}

          {:error, reason} ->
            {:error, reason}
        end
    end
  end

  @doc """
  部屋招待を辞退する。
  """
  @spec decline_room_invite(String.t(), String.t()) :: {:ok, :declined} | {:error, :not_found}
  def decline_room_invite(user_id, invite_id) do
    invite = Repo.get_by(RoomInvite, id: invite_id, invitee_id: user_id)

    case invite do
      nil ->
        {:error, :not_found}

      %RoomInvite{} = inv ->
        now = DateTime.utc_now() |> DateTime.truncate(:microsecond)

        inv
        |> RoomInvite.changeset(%{status: "DECLINED", responded_at: now})
        |> Repo.update!()

        {:ok, :declined}
    end
  end

  @doc """
  部屋へ参加する（プレイヤーまたは観戦者）。
  """
  @spec join_room(String.t(), String.t(), String.t()) ::
          {:ok, %{room: map(), realtime: map()}}
          | {:error,
             :not_found
             | :room_full
             | :conflict
             | :game_in_progress
             | :invalid_payload
             | :invalid_phase}
  def join_room(user_id, room_id, participant_type \\ "PLAYER")

  def join_room(user_id, room_id, participant_type)
      when participant_type in ["PLAYER", "SPECTATOR"] do
    room =
      Repo.get(Room, room_id)
      |> Repo.preload(members: :user)

    case room do
      nil ->
        {:error, :not_found}

      %Room{status: "CLOSED"} ->
        {:error, :not_found}

      %Room{status: "IN_GAME"} when participant_type != "SPECTATOR" ->
        {:error, :game_in_progress}

      %Room{} ->
        now = DateTime.utc_now() |> DateTime.truncate(:microsecond)
        existing_member = Enum.find(room.members, &(&1.user_id == user_id))
        player_count = Enum.count(room.members, &(&1.participant_type == "PLAYER"))

        cond do
          existing_member && existing_member.participant_type != participant_type ->
            with {:ok, room_state} <-
                   WordWolf.Games.GameServer.change_participation(
                     room_id,
                     user_id,
                     participant_type
                   ) do
              {:ok,
               %{
                 room: room_state,
                 realtime: %{roomTopic: "room:#{room_id}", userTopic: "user:#{user_id}"}
               }}
            end

          is_nil(existing_member) and participant_type == "PLAYER" and
              player_count >= room.max_players ->
            {:error, :room_full}

          is_nil(existing_member) ->
            member_id = "rmem_#{Ecto.UUID.generate() |> String.slice(0, 16)}"

            %RoomMember{
              id: member_id,
              room_id: room_id,
              user_id: user_id,
              participant_type: participant_type,
              ready: false,
              connected: true,
              joined_at: now
            }
            |> Repo.insert!()

            updated_room =
              Repo.get!(Room, room_id)
              |> Repo.preload(members: :user)

            room_state = to_room_state(updated_room)
            broadcast_room_state(room_id)

            {:ok,
             %{
               room: room_state,
               realtime: %{
                 roomTopic: "room:#{room_id}",
                 userTopic: "user:#{user_id}"
               }
             }}

          true ->
            {:ok,
             %{
               room: to_room_state(room),
               realtime: %{
                 roomTopic: "room:#{room_id}",
                 userTopic: "user:#{user_id}"
               }
             }}
        end
    end
  end

  def join_room(_user_id, _room_id, _participant_type), do: {:error, :invalid_payload}

  @doc "GameServerで待機中と確認した参加方法の変更を、ホスト移譲と同時に保存する。"
  @spec persist_participation(String.t(), String.t(), WordWolf.Contract.Types.participant_type()) ::
          {:ok, WordWolf.Contract.Types.room_state()} | {:error, atom()}
  def persist_participation(room_id, user_id, participant_type) do
    Repo.transaction(fn ->
      room = Repo.one(from(r in Room, where: r.id == ^room_id, lock: "FOR UPDATE"))
      member = Repo.get_by(RoomMember, room_id: room_id, user_id: user_id)
      if is_nil(room) or is_nil(member), do: Repo.rollback(:not_found)
      if room.status != "WAITING", do: Repo.rollback(:game_in_progress)

      players =
        Repo.all(
          from(m in RoomMember,
            where: m.room_id == ^room_id and m.participant_type == "PLAYER",
            order_by: [asc: m.joined_at, asc: m.id]
          )
        )

      if participant_type == "PLAYER" and member.participant_type != "PLAYER" and
           length(players) >= room.max_players,
         do: Repo.rollback(:room_full)

      if participant_type == "SPECTATOR" and room.host_user_id == user_id do
        new_host = Enum.find(players, &(&1.user_id != user_id))
        if is_nil(new_host), do: Repo.rollback(:invalid_phase)
        room |> Room.changeset(%{host_user_id: new_host.user_id}) |> Repo.update!()
        persist_member_ready(room_id, new_host.user_id, true)
      end

      member
      |> Ecto.Changeset.change(participant_type: participant_type, ready: false)
      |> Repo.update!()

      Repo.get!(Room, room_id) |> to_room_state()
    end)
  end

  @doc """
  部屋から退出する（POST /rooms/{roomId}/leave）。
  ホストが退出した場合は最古参プレイヤーへホストを移譲し、誰もいなくなれば部屋を CLOSED にする。
  """
  @spec leave_room(String.t(), String.t()) :: {:ok, :left} | {:error, :not_found}
  def leave_room(user_id, room_id) when is_binary(user_id) and is_binary(room_id) do
    room =
      Repo.get(Room, room_id)
      |> Repo.preload([:members, members: :user])

    case room do
      nil ->
        {:error, :not_found}

      %Room{status: "CLOSED"} ->
        {:error, :not_found}

      %Room{} = r ->
        is_member = Enum.any?(r.members, &(&1.user_id == user_id))

        if not is_member do
          {:error, :not_found}
        else
          now = DateTime.utc_now() |> DateTime.truncate(:microsecond)

          # メンバーから削除
          from(m in RoomMember, where: m.room_id == ^room_id and m.user_id == ^user_id)
          |> Repo.delete_all()

          remaining_members = Enum.reject(r.members, &(&1.user_id == user_id))
          remaining_players = Enum.filter(remaining_members, &(&1.participant_type == "PLAYER"))

          cond do
            # プレイヤーが誰もいなくなった場合 → 部屋を CLOSED にし、招待を破棄
            Enum.empty?(remaining_players) ->
              r
              |> Room.changeset(%{status: "CLOSED", closed_at: now})
              |> Repo.update!()

              from(i in RoomInvite,
                where: i.room_id == ^room_id and i.status == "PENDING"
              )
              |> Repo.update_all(set: [status: "DECLINED", responded_at: now])

              broadcast_room_state(room_id)

              {:ok, :left}

            # ホストが抜けた場合 → 最古参プレイヤーにホスト移譲
            r.host_user_id == user_id ->
              new_host = Enum.min_by(remaining_players, & &1.joined_at)

              r
              |> Room.changeset(%{host_user_id: new_host.user_id})
              |> Repo.update!()

              # 新しいホストは常に準備完了（契約 asyncapi.yaml onRoomReady）。REST の ready もそろえる
              persist_member_ready(room_id, new_host.user_id, true)

              broadcast_room_state(room_id)

              {:ok, :left}

            # 一般メンバーが抜けた場合
            true ->
              broadcast_room_state(room_id)

              {:ok, :left}
          end
        end
    end
  end

  @doc """
  RoomInvite レコードを契約スキーマ RoomInvite の Map に変換する。
  """
  def to_room_invite_map(%RoomInvite{} = invite) do
    inviter = invite.inviter || Repo.get(User, invite.inviter_id)
    room = invite.room || Repo.get(Room, invite.room_id)

    %{
      inviteId: invite.id,
      roomId: room.id,
      inviteCode: room.invite_code,
      roomName: room.room_name,
      from: Accounts.to_user_public(inviter),
      sentAt: DateTime.to_iso8601(invite.sent_at)
    }
  end

  @doc """
  Room レコードを契約スキーマ RoomState の Map に変換する。
  """
  def to_room_state(%Room{} = room) do
    room = Repo.preload(room, [:members, members: :user])

    players =
      room.members
      |> Enum.filter(&(&1.participant_type == "PLAYER"))
      |> Enum.map(fn m ->
        user = m.user || Repo.get(User, m.user_id)
        pub = Accounts.to_user_public(user)

        Map.merge(pub, %{
          isHost: room.host_user_id == m.user_id,
          ready: m.ready or room.host_user_id == m.user_id,
          connected: m.connected
        })
      end)

    spectators =
      room.members
      |> Enum.filter(&(&1.participant_type == "SPECTATOR"))
      |> Enum.map(fn m ->
        user = m.user || Repo.get(User, m.user_id)
        Accounts.to_user_public(user)
      end)

    %{
      roomId: room.id,
      roomName: room.room_name,
      status: room.status,
      inviteCode: room.invite_code,
      inviteUrl: "https://example.com/join/#{room.invite_code}",
      hostUserId: room.host_user_id,
      settings: %{
        roomName: room.room_name,
        genres: room.genres,
        comebackEnabled: room.comeback_enabled,
        discussionMs: room.discussion_ms,
        voteMs: room.vote_ms,
        minPlayers: room.min_players,
        maxPlayers: room.max_players,
        reconnectGraceMs: room.reconnect_grace_ms
      },
      players: players,
      spectators: spectators,
      currentGameId: current_game_id(room)
    }
  end

  defp current_game_id(%Room{status: "IN_GAME", id: room_id}) do
    Repo.one(
      from(g in WordWolf.Games.Game,
        where: g.room_id == ^room_id and is_nil(g.aborted_at),
        order_by: [desc: g.started_at, desc: g.id],
        limit: 1,
        select: g.id
      )
    )
  end

  defp current_game_id(_room), do: nil

  @doc "ロビー復帰時の部屋状態と準備状態をまとめて保存する。ホストは常に準備完了。"
  @spec persist_lobby(String.t(), String.t()) :: :ok
  def persist_lobby(room_id, host_user_id) do
    Repo.transaction(fn ->
      from(r in Room, where: r.id == ^room_id and r.status != "CLOSED")
      |> Repo.update_all(set: [status: "WAITING", updated_at: DateTime.utc_now()])

      from(m in RoomMember, where: m.room_id == ^room_id)
      |> Repo.update_all(set: [ready: false, updated_at: DateTime.utc_now()])

      persist_member_ready(room_id, host_user_id, true)
    end)

    :ok
  end

  @doc """
  部屋設定を変更する（PUT /rooms/{roomId}/settings, updateRoomSettings）。
  ホストのみ・WAITING（LOBBY）中のみ。ゲーム開始と設定変更をGameServerで直列化する。
  """
  @spec update_settings(String.t(), String.t(), map()) ::
          {:ok, map()} | {:error, :not_found | :not_host | :invalid_phase | :invalid_payload}
  def update_settings(user_id, room_id, params) when is_map(params) do
    WordWolf.Games.GameServer.update_settings(room_id, user_id, params)
  end

  @doc "ホストがLOBBY中の参加者を退出させる。開始との直列化はGameServerが行う。"
  @spec persist_remove_member(String.t(), String.t(), String.t()) ::
          {:ok, WordWolf.Contract.Types.room_state()} | {:error, atom()}
  def persist_remove_member(user_id, room_id, target_user_id) do
    Repo.transaction(fn ->
      room = Repo.one(from(r in Room, where: r.id == ^room_id, lock: "FOR UPDATE"))
      if is_nil(room) or room.status == "CLOSED", do: Repo.rollback(:not_found)
      if room.host_user_id != user_id, do: Repo.rollback(:not_host)
      if room.status != "WAITING", do: Repo.rollback(:invalid_phase)
      if user_id == target_user_id, do: Repo.rollback(:invalid_payload)

      {count, _} =
        from(m in RoomMember, where: m.room_id == ^room_id and m.user_id == ^target_user_id)
        |> Repo.delete_all()

      if count == 0, do: Repo.rollback(:not_found)
      to_room_state(room)
    end)
  end

  @spec persist_settings(String.t(), String.t(), map()) ::
          {:ok, WordWolf.Contract.Types.room_state()} | {:error, atom()}
  def persist_settings(user_id, room_id, params) do
    with {:ok, room} <- fetch_open_room(room_id),
         :ok <- ensure_host(room, user_id),
         :ok <- ensure_waiting(room),
         {:ok, attrs} <- validate_settings(params, room) do
      updated =
        room
        |> Room.changeset(attrs)
        |> Repo.update!()
        |> Repo.preload([:members, members: :user], force: true)

      {:ok, to_room_state(updated)}
    end
  end

  defp fetch_open_room(room_id) do
    case Repo.get(Room, room_id) do
      nil -> {:error, :not_found}
      %Room{status: "CLOSED"} -> {:error, :not_found}
      %Room{} = room -> {:ok, Repo.preload(room, [:members, members: :user])}
    end
  end

  defp ensure_host(%Room{host_user_id: id}, id), do: :ok
  defp ensure_host(_room, _user_id), do: {:error, :not_host}

  defp ensure_waiting(%Room{status: "WAITING"}), do: :ok
  defp ensure_waiting(_room), do: {:error, :invalid_phase}

  defp valid_genres?(genres) do
    is_list(genres) and genres != [] and genres == Enum.uniq(genres) and
      Enum.all?(genres, &(&1 in WordWolf.Contract.Types.room_genre_values()))
  end

  defp validate_settings(params, room) do
    room_name = Map.get(params, "roomName", room.room_name)
    genres = Map.get(params, "genres", room.genres)
    comeback_enabled = Map.get(params, "comebackEnabled", room.comeback_enabled)
    discussion_ms = Map.get(params, "discussionMs", room.discussion_ms)
    vote_ms = Map.get(params, "voteMs", room.vote_ms)

    valid? =
      is_binary(room_name) and String.length(String.trim(room_name)) in 2..30 and
        valid_genres?(genres) and is_boolean(comeback_enabled) and
        valid_timing?(discussion_ms, vote_ms)

    if valid? do
      {:ok,
       %{
         room_name: String.trim(room_name),
         genres: genres,
         comeback_enabled: comeback_enabled,
         discussion_ms: discussion_ms,
         vote_ms: vote_ms
       }}
    else
      {:error, :invalid_payload}
    end
  end

  defp valid_timing?(discussion_ms, vote_ms) do
    is_integer(discussion_ms) and discussion_ms in 30_000..900_000 and
      is_integer(vote_ms) and vote_ms in 10_000..180_000
  end

  @spec spectator?(String.t(), String.t()) :: boolean()
  def spectator?(room_id, user_id) do
    Repo.exists?(
      from(m in RoomMember,
        where:
          m.room_id == ^room_id and m.user_id == ^user_id and m.participant_type == "SPECTATOR"
      )
    )
  end

  @doc """
  準備状態（ready）をDBへ書き写すだけの関数。判断も配信もしない。

  ready の正本は WordWolf.Games.GameServer（room.ready を受けて直列化する）で、ここは
  REST の GET /rooms/:roomId が古い値を返さないようにするための写像。検証や配信を
  したい場合は GameServer 側を呼ぶこと。
  """
  @spec persist_member_ready(String.t(), String.t(), boolean()) :: :ok
  def persist_member_ready(room_id, user_id, ready)
      when is_binary(room_id) and is_binary(user_id) and is_boolean(ready) do
    from(m in RoomMember, where: m.room_id == ^room_id and m.user_id == ^user_id)
    |> Repo.update_all(set: [ready: ready, updated_at: DateTime.utc_now()])

    :ok
  end

  # room.state の形は契約 asyncapi.yaml の RoomStateSnapshot で、生成できるのは
  # GameServer（フェーズ・締切・投票進捗を持つ）だけ。メンバーが変わったら読み直させる。
  defp broadcast_room_state(room_id) do
    case WordWolf.Games.GameSupervisor.ensure_started(room_id) do
      {:ok, server} -> WordWolf.Games.GameServer.refresh(server)
      {:error, _reason} -> :ok
    end
  end

  def preview(%Room{} = room) do
    players = Enum.count(room.members, &(&1.participant_type == "PLAYER"))
    spectators = Enum.count(room.members, &(&1.participant_type == "SPECTATOR"))

    %{
      roomId: room.id,
      inviteCode: room.invite_code,
      roomName: room.room_name,
      status: room.status,
      genres: room.genres,
      playerCount: players,
      maxPlayers: room.max_players,
      spectatorCount: spectators,
      joinable: room.status == "WAITING" and players < room.max_players
    }
  end
end
