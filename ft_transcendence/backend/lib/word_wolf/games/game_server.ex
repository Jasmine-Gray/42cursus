defmodule WordWolf.Games.GameServer do
  @moduledoc false
  use GenServer, restart: :transient
  alias WordWolf.Chat
  alias WordWolf.Games
  alias WordWolf.Games.GameServer.{Presenter, RoomLoader}
  alias WordWolf.Games.GameServer.Chat, as: ChatRules
  alias WordWolf.Rooms
  alias WordWolfWeb.Endpoint

  # 固定長フェーズの既定値。部屋ごとに設定できる discussion_ms / vote_ms と違い契約上は固定だが、
  # テストが DEALING の3秒を実時間待たずに DISCUSSION へ進めるよう Application env で上書きできる。
  @default_dealing_ms 3_000
  @default_result_ms 30_000

  @type actor :: {String.t(), String.t()}

  def start_link(room_id), do: GenServer.start_link(__MODULE__, room_id, name: via(room_id))

  @spec connect(GenServer.server(), String.t(), String.t(), pid()) ::
          {:ok, map()} | {:error, atom()}
  def connect(server, user_id, connection_id, channel_pid),
    do: GenServer.call(server, {:connect, user_id, connection_id, channel_pid})

  def disconnect(server, actor), do: GenServer.cast(server, {:disconnect, actor})

  @doc """
  部屋のメンバーをDBから読み直して room.state を配信し直す。
  RESTでの参加・退出（WordWolf.Rooms）から呼ぶ。room.state の形は契約の
  RoomStateSnapshot で、フェーズや締切を持つここでしか作れない。
  """
  @spec refresh(GenServer.server()) :: :ok
  def refresh(server), do: GenServer.cast(server, :refresh)
  def state(server, actor), do: GenServer.call(server, {:state, actor})

  def valid_connection?(server, actor),
    do: GenServer.call(server, {:valid_connection?, actor})

  def private_state(server, actor), do: GenServer.call(server, {:private_state, actor})

  def set_ready(server, actor, ready), do: GenServer.call(server, {:set_ready, actor, ready})

  @doc """
  議論スキップの賛成をトグルする（契約 asyncapi.yaml onDiscussionSkip）。
  プレイヤー全員が賛成した時点で VOTING へ遷移する。
  """
  def set_skip(server, actor, skip), do: GenServer.call(server, {:set_skip, actor, skip})

  @spec comeback_draft(GenServer.server(), actor(), String.t()) ::
          {:ok, WordWolf.Contract.Types.comeback_state()} | {:error, atom()}
  def comeback_draft(server, actor, content),
    do: GenServer.call(server, {:comeback_draft, actor, content})

  @spec comeback_submit(GenServer.server(), actor(), String.t()) ::
          {:ok, map()} | {:error, atom()}
  def comeback_submit(server, actor, content),
    do: GenServer.call(server, {:comeback_submit, actor, content})

  def send_chat(server, actor, client_id, content),
    do: GenServer.call(server, {:send_chat, actor, client_id, content})

  def start_game(room_id, user_id) do
    with {:ok, pid} <- WordWolf.Games.GameSupervisor.ensure_started(room_id),
         do: GenServer.call(pid, {:start_game, user_id})
  end

  @spec change_participation(String.t(), String.t(), WordWolf.Contract.Types.participant_type()) ::
          {:ok, WordWolf.Contract.Types.room_state()} | {:error, atom()}
  def change_participation(room_id, user_id, participant_type) do
    with {:ok, pid} <- WordWolf.Games.GameSupervisor.ensure_started(room_id),
         do: GenServer.call(pid, {:change_participation, user_id, participant_type})
  end

  @spec update_settings(String.t(), String.t(), map()) ::
          {:ok, WordWolf.Contract.Types.room_state()} | {:error, atom()}
  def update_settings(room_id, user_id, params) do
    with {:ok, pid} <- WordWolf.Games.GameSupervisor.ensure_started(room_id),
         do: GenServer.call(pid, {:update_settings, user_id, params})
  end

  @spec remove_member(String.t(), String.t(), String.t()) ::
          {:ok, WordWolf.Contract.Types.room_state()} | {:error, atom()}
  def remove_member(room_id, user_id, target_user_id) do
    with {:ok, pid} <- WordWolf.Games.GameSupervisor.ensure_started(room_id),
         do: GenServer.call(pid, {:remove_member, user_id, target_user_id})
  end

  @spec spectator_assignments(String.t(), String.t()) ::
          {:ok, WordWolf.Contract.Types.spectator_assignments()} | {:error, atom()}
  def spectator_assignments(game_id, user_id),
    do: via_game(game_id, {:spectator_assignments, game_id, user_id})

  def cast_vote(game_id, user_id, target_user_id),
    do: via_game(game_id, {:cast_vote, user_id, target_user_id})

  @doc """
  REST（GET /games/{gameId} ほか）から game_id で引く参照系。Channel と違って
  connectionId を持たないので、接続の有効性ではなく「その部屋の参加者か」だけで認可する。
  進行中のゲームはGenServerが正本なので、DBではなくプロセスに問い合わせる。
  """
  @spec game_public_state(String.t(), String.t()) :: {:ok, map()} | {:error, atom()}
  def game_public_state(game_id, user_id), do: via_game(game_id, {:game_public_state, user_id})

  @spec assignment(String.t(), String.t()) :: {:ok, map()} | {:error, atom()}
  def assignment(game_id, user_id), do: via_game(game_id, {:assignment, user_id})

  @spec game_result(String.t(), String.t()) :: {:ok, map()} | {:error, atom()}
  def game_result(game_id, user_id), do: via_game(game_id, {:game_result, user_id})

  # ゲームは Registry に {:game, game_id} で登録される（start_game 時）。
  # 決着して LOBBY に戻ると解除されるので、そのときは :not_found。
  defp via_game(game_id, message) do
    case Registry.lookup(WordWolf.GameRegistry, {:game, game_id}) do
      [{pid, _value}] -> GenServer.call(pid, message)
      [] -> {:error, :not_found}
    end
  end

  @impl true
  def init(room_id) do
    case RoomLoader.load(room_id) do
      {:ok, state} -> {:ok, state}
      {:error, reason} -> {:stop, reason}
    end
  end

  @impl true
  def handle_call({:connect, user_id, connection_id, channel_pid}, _from, state) do
    case RoomLoader.refresh_member(state, user_id) do
      {:ok, state} ->
        state = replace_connection(state, user_id)
        monitor_ref = Process.monitor(channel_pid)
        connection = %{id: connection_id, pid: channel_pid, monitor_ref: monitor_ref}

        state = %{
          state
          | connections: Map.put(state.connections, user_id, connection),
            monitors: Map.put(state.monitors, monitor_ref, {user_id, connection_id})
        }

        state = state |> set_connected(user_id, true) |> maybe_resume()

        if state.game_id && Map.has_key?(state.spectators, user_id) do
          Games.record_spectator(state.game_id, user_id)
        end

        broadcast(state, "player.connection_changed", %{userId: user_id, connected: true})
        send_private_state(state, user_id)
        {:reply, {:ok, Presenter.public_state(state)}, state}

      {:error, :forbidden} ->
        {:reply, {:error, :forbidden}, state}
    end
  end

  def handle_call({:state, actor}, _from, state) do
    with :ok <- validate_connection(state, actor) do
      send_private_state(state, elem(actor, 0))
      {:reply, {:ok, Presenter.public_state(state)}, state}
    else
      error -> {:reply, error, state}
    end
  end

  def handle_call({:valid_connection?, actor}, _from, state) do
    {:reply, validate_connection(state, actor) == :ok, state}
  end

  def handle_call({:private_state, actor}, _from, state) do
    with :ok <- validate_connection(state, actor),
         {:ok, payload} <- Presenter.private_state(state, elem(actor, 0)) do
      {:reply, {:ok, payload}, state}
    else
      error -> {:reply, error, state}
    end
  end

  def handle_call({:update_settings, user_id, params}, _from, state) do
    with :ok <- require_phase(state, "LOBBY"),
         {:ok, room_state} <- Rooms.persist_settings(user_id, state.room_id, params) do
      state = RoomLoader.reload_members(state)
      broadcast(state, "room.state", Presenter.public_state(state))
      {:reply, {:ok, room_state}, state}
    else
      error -> {:reply, error, state}
    end
  end

  def handle_call({:remove_member, user_id, target_user_id}, _from, state) do
    with :ok <- require_phase(state, "LOBBY"),
         {:ok, room_state} <- Rooms.persist_remove_member(user_id, state.room_id, target_user_id) do
      state = RoomLoader.reload_members(state)

      state =
        case Map.get(state.connections, target_user_id) do
          nil ->
            state

          connection ->
            send(connection.pid, {:room_removed, connection.id})
            disconnect_actor(state, {target_user_id, connection.id})
        end

      Endpoint.broadcast("user:#{target_user_id}", "room.removed", %{
        roomId: state.room_id,
        roomName: room_state.roomName,
        reason: "REMOVED_BY_HOST"
      })

      broadcast(state, "room.state", Presenter.public_state(state))
      {:reply, {:ok, room_state}, state}
    else
      error -> {:reply, error, state}
    end
  end

  def handle_call({:spectator_assignments, game_id, user_id}, _from, state) do
    cond do
      state.game_id != game_id ->
        {:reply, {:error, :not_found}, state}

      not Map.has_key?(state.roles, user_id) and Rooms.spectator?(state.room_id, user_id) ->
        {:reply, {:ok, Presenter.spectator_assignments(state)}, state}

      true ->
        {:reply, {:error, :forbidden}, state}
    end
  end

  def handle_call({:change_participation, user_id, participant_type}, _from, state) do
    if state.phase == "LOBBY" do
      case Rooms.persist_participation(state.room_id, user_id, participant_type) do
        {:ok, room_state} ->
          state = RoomLoader.reload_members(state)
          broadcast(state, "room.state", Presenter.public_state(state))
          {:reply, {:ok, room_state}, state}

        error ->
          {:reply, error, state}
      end
    else
      {:reply, {:error, :game_in_progress}, state}
    end
  end

  def handle_call({:set_ready, actor, ready}, _from, state) do
    with :ok <- validate_action(state, actor),
         :ok <- require_phase(state, "LOBBY"),
         :ok <- require_player(state, elem(actor, 0)) do
      user_id = elem(actor, 0)

      # ホストは常に準備完了（契約 asyncapi.yaml onRoomReady）。取り消しは無視する
      ready = ready or user_id == state.host_user_id
      players = Map.update!(state.players, user_id, &Map.put(&1, :ready, ready))
      state = %{state | players: players}

      # REST の GET /rooms/:roomId が古い ready を返さないようDBにも写す（レーンC）
      Rooms.persist_member_ready(state.room_id, user_id, ready)
      broadcast(state, "room.state", Presenter.public_state(state))
      {:reply, {:ok, %{ready: ready}}, state}
    else
      error -> {:reply, error, state}
    end
  end

  def handle_call({:set_skip, actor, skip}, _from, state) do
    with :ok <- validate_action(state, actor),
         :ok <- require_phase(state, "DISCUSSION"),
         :ok <- require_player(state, elem(actor, 0)) do
      user_id = elem(actor, 0)

      skip_votes =
        if skip,
          do: MapSet.put(state.skip_votes, user_id),
          else: MapSet.delete(state.skip_votes, user_id)

      state = %{state | skip_votes: skip_votes}
      broadcast(state, "skip.progress", Presenter.skip_progress(state))

      # 全員一致で締切を待たず投票へ。最低議論時間は設けない（契約 §議論のスキップ）
      state =
        if MapSet.size(skip_votes) == map_size(state.players) and map_size(state.players) > 0,
          do: transition(state, "VOTING", state.settings.vote_ms),
          else: state

      {:reply, {:ok, %{skip: skip}}, state}
    else
      error -> {:reply, error, state}
    end
  end

  def handle_call({:send_chat, actor, client_id, content}, _from, state) do
    user_id = elem(actor, 0)

    # フェーズ・送信権限・レート制限・重複排除はGenServerの状態で判断し（接続単位だと
    # 再接続で復活してしまう）、本文の保存だけ WordWolf.Chat（レーンD）へ渡す。
    with :ok <- validate_action(state, actor),
         :ok <- require_phase(state, "DISCUSSION"),
         :ok <- require_player(state, user_id),
         {:ok, state, content} <- ChatRules.accept(state, user_id, client_id, content),
         {:ok, message} <- publish_chat(state, user_id, content) do
      payload = Map.put(message, :clientMessageId, client_id)

      broadcast(state, "chat.published", payload)
      {:reply, {:ok, %{messageId: message.messageId}}, state}
    else
      error -> {:reply, error, state}
    end
  end

  def handle_call({:start_game, user_id}, _from, state) do
    with :ok <- require_phase(state, "LOBBY"),
         :ok <- require_host(state, user_id),
         :ok <- ready_to_start(state),
         {:ok, group} <- choose_word_group(state) do
      [majority_word, minority_word] = Enum.take_random(group.words, 2)
      player_ids = Map.keys(state.players)
      wolf_id = Enum.at(player_ids, random_index(length(player_ids)))
      roles = Map.new(player_ids, &{&1, if(&1 == wolf_id, do: "WORD_WOLF", else: "CITIZEN")})

      words =
        Map.new(
          player_ids,
          &{&1, if(&1 == wolf_id, do: minority_word, else: majority_word)}
        )

      game_id = id("game")
      {:ok, _} = Registry.register(WordWolf.GameRegistry, {:game, game_id}, nil)

      state = %{
        state
        | game_id: game_id,
          started_at: DateTime.utc_now(),
          word_group: group,
          roles: roles,
          words: words,
          votes: %{},
          skip_votes: MapSet.new(),
          result: nil,
          comeback: nil,
          comeback_result: nil,
          comeback_updated_at: nil,
          chat_rate: %{},
          seen_messages: MapSet.new()
      }

      # transition が phase_version と deadline を確定させてから games 行を作る。
      # この時点ではまだ行が無いので、transition 内の update_phase は空振りする（0行更新）。
      state = transition(state, "DEALING", dealing_ms())

      {:ok, _game} =
        Games.create_game(%{
          game_id: game_id,
          room_id: state.room_id,
          word_group_id: group.id,
          phase: state.phase,
          phase_version: state.phase_version,
          deadline: state.deadline,
          roles: roles,
          words: words,
          spectator_ids: Map.keys(state.spectators)
        })

      Enum.each(player_ids, &send_private_state(state, &1))
      # フレンドの「対戦中」表示はDB書き込みと配信を伴うので、reply を遅らせないよう
      # handle_info へ1呼吸ずらす（テスト用の短い dealing_ms とレースしないため）。
      send(self(), {:presence_game_started, player_ids})
      {:reply, {:ok, %{gameId: game_id}}, state}
    else
      error -> {:reply, error, state}
    end
  end

  def handle_call({:cast_vote, user_id, target_user_id}, _from, state) do
    with :ok <- require_not_paused(state),
         :ok <- require_phase(state, "VOTING"),
         :ok <- require_before_deadline(state),
         :ok <- require_player(state, user_id),
         :ok <- valid_target(state, user_id, target_user_id),
         :ok <- not_voted(state, user_id) do
      state = %{state | votes: Map.put(state.votes, user_id, target_user_id)}
      Games.record_vote(state.game_id, user_id, target_user_id)
      progress = Presenter.vote_progress(state)
      broadcast(state, "vote.progress", progress)

      state =
        if map_size(state.votes) == map_size(state.players), do: finish_game(state), else: state

      {:reply, {:ok, progress}, state}
    else
      error -> {:reply, error, state}
    end
  end

  def handle_call({:comeback_draft, actor, content}, _from, state) do
    now = System.monotonic_time(:millisecond)

    with :ok <- validate_comeback(state, actor),
         :ok <- validate_answer(content, true),
         :ok <- draft_rate(state, now) do
      comeback = %{state.comeback | content: content}
      broadcast(state, "comeback.updated", comeback)
      {:reply, {:ok, comeback}, %{state | comeback: comeback, comeback_updated_at: now}}
    else
      error -> {:reply, error, state}
    end
  end

  def handle_call({:comeback_submit, actor, content}, _from, state) do
    with :ok <- validate_comeback(state, actor),
         :ok <- validate_answer(content, false) do
      state = %{state | comeback: %{state.comeback | content: String.trim(content)}}
      {:reply, {:ok, %{}}, finish_comeback(state, false)}
    else
      error -> {:reply, error, state}
    end
  end

  # --- REST 参照系（game_id 起点。接続ではなく参加者かどうかで認可する）-----------

  def handle_call({:game_public_state, user_id}, _from, state) do
    reply =
      if participant?(state, user_id),
        do: {:ok, Presenter.game_public_state(state)},
        else: {:error, :forbidden}

    {:reply, reply, state}
  end

  def handle_call({:assignment, user_id}, _from, state) do
    reply =
      cond do
        not participant?(state, user_id) -> {:error, :forbidden}
        # 観戦者にお題は無い（契約 getMyAssignment は NOT_A_PLAYER で拒否する）
        not Map.has_key?(state.players, user_id) -> {:error, :not_a_player}
        true -> Presenter.private_state(state, user_id) |> assignment_reply()
      end

    {:reply, reply, state}
  end

  def handle_call({:game_result, user_id}, _from, state) do
    reply =
      cond do
        not participant?(state, user_id) -> {:error, :forbidden}
        # 契約 getGameResult: RESULT フェーズ以降だけ取得できる
        is_nil(state.result) -> {:error, :invalid_phase}
        true -> {:ok, Presenter.result_for(state, user_id)}
      end

    {:reply, reply, state}
  end

  defp assignment_reply({:ok, nil}), do: {:error, :invalid_phase}
  defp assignment_reply({:ok, payload}), do: {:ok, payload}

  defp participant?(state, user_id),
    do: Map.has_key?(state.players, user_id) or Map.has_key?(state.spectators, user_id)

  @impl true
  def handle_cast({:disconnect, actor}, state), do: {:noreply, disconnect_actor(state, actor)}

  def handle_cast(:refresh, state) do
    state = RoomLoader.reload_members(state)
    broadcast(state, "room.state", Presenter.public_state(state))
    {:noreply, state}
  end

  @impl true
  def handle_info({:DOWN, ref, :process, _pid, _reason}, state) do
    case Map.pop(state.monitors, ref) do
      {nil, _} ->
        {:noreply, state}

      {{user_id, connection_id}, monitors} ->
        {:noreply, disconnect_actor(%{state | monitors: monitors}, {user_id, connection_id})}
    end
  end

  def handle_info({:phase_timeout, version}, %{phase_version: version, paused: false} = state) do
    state =
      case state.phase do
        "DEALING" -> transition(state, "DISCUSSION", state.settings.discussion_ms)
        "DISCUSSION" -> transition(state, "VOTING", state.settings.vote_ms)
        "VOTING" -> finish_game(state)
        "COMEBACK" -> finish_comeback(state, true)
        "RESULT" -> reset_lobby(state)
        _ -> state
      end

    {:noreply, state}
  end

  def handle_info({:phase_timeout, _version}, state), do: {:noreply, state}

  def handle_info({:reconnect_timeout, version}, %{phase_version: version, paused: true} = state) do
    if state.game_id, do: Games.abort_game(state.game_id)

    broadcast(state, "game.aborted", %{
      gameId: state.game_id,
      reason: "RECONNECT_TIMEOUT",
      nextPhase: "LOBBY"
    })

    {:noreply, reset_lobby(state)}
  end

  def handle_info({:reconnect_timeout, _version}, state), do: {:noreply, state}

  # フレンドのオンライン状態（WordWolf.Presence）。start_game / reset_lobby からの自己送信。
  # handle_call の応答やフェーズ遷移のタイマーをブロックしないよう、ここで受けてから処理する。
  def handle_info({:presence_game_started, player_ids}, state) do
    WordWolf.Presence.game_started(player_ids)
    {:noreply, state}
  end

  def handle_info({:presence_game_ended, player_ids}, state) do
    WordWolf.Presence.game_ended(player_ids)
    {:noreply, state}
  end

  defp disconnect_actor(state, {user_id, connection_id}) do
    case Map.get(state.connections, user_id) do
      %{id: ^connection_id, monitor_ref: ref} ->
        Process.demonitor(ref, [:flush])

        state = %{
          state
          | connections: Map.delete(state.connections, user_id),
            monitors: Map.delete(state.monitors, ref)
        }

        state = set_connected(state, user_id, false)
        broadcast(state, "player.connection_changed", %{userId: user_id, connected: false})
        maybe_pause(state, user_id)

      _ ->
        state
    end
  end

  defp maybe_pause(state, user_id) do
    if state.phase in ["DEALING", "DISCUSSION", "VOTING", "COMEBACK"] and
         Map.has_key?(state.players, user_id) do
      disconnected = MapSet.put(state.disconnected, user_id)

      if state.paused do
        state = %{state | disconnected: disconnected}
        broadcast_paused(state)
        state
      else
        cancel_timer(state.timer_ref)
        remaining = max(DateTime.diff(state.deadline, DateTime.utc_now(), :millisecond), 0)

        deadline =
          DateTime.add(DateTime.utc_now(), state.settings.reconnect_grace_ms, :millisecond)

        ref =
          Process.send_after(
            self(),
            {:reconnect_timeout, state.phase_version},
            state.settings.reconnect_grace_ms
          )

        state = %{
          state
          | paused: true,
            disconnected: disconnected,
            paused_remaining_ms: remaining,
            deadline: nil,
            timer_ref: nil,
            reconnect_deadline: deadline,
            reconnect_timer_ref: ref
        }

        Games.set_paused(state.game_id, true, deadline)
        broadcast_paused(state)
        state
      end
    else
      state
    end
  end

  defp maybe_resume(%{paused: true} = state) do
    disconnected =
      Enum.reduce(state.disconnected, state.disconnected, fn id, acc ->
        if Map.has_key?(state.connections, id), do: MapSet.delete(acc, id), else: acc
      end)

    if MapSet.size(disconnected) == 0 do
      cancel_timer(state.reconnect_timer_ref)
      {deadline, ref} = schedule(state.paused_remaining_ms, state.phase_version)

      state = %{
        state
        | paused: false,
          disconnected: disconnected,
          deadline: deadline,
          timer_ref: ref,
          reconnect_deadline: nil,
          reconnect_timer_ref: nil,
          paused_remaining_ms: nil
      }

      Games.set_paused(state.game_id, false, nil)

      broadcast(state, "game.resumed", %{
        phase: state.phase,
        phaseVersion: state.phase_version,
        deadline: Presenter.iso(deadline)
      })

      broadcast(state, "room.state", Presenter.public_state(state))
      state
    else
      state = %{state | disconnected: disconnected}
      broadcast_paused(state)
      state
    end
  end

  defp maybe_resume(state), do: state

  defp transition(state, phase, duration_ms) do
    cancel_timer(state.timer_ref)
    version = state.phase_version + 1
    {deadline, ref} = schedule(duration_ms, version)

    # 議論スキップの賛成はDISCUSSION限りなので、フェーズが変わる時点で必ず捨てる
    # （契約 §議論のスキップ）。入場・退出で人数が変わっても持ち越さない。
    state = %{
      state
      | phase: phase,
        phase_version: version,
        deadline: deadline,
        timer_ref: ref,
        skip_votes: MapSet.new()
    }

    # DBは写像。開始直後のDEALINGだけは行がまだ無く0行更新になる（create_game が続けて書く）。
    if state.game_id, do: Games.update_phase(state.game_id, phase, version, deadline)

    broadcast(state, "phase.changed", %{
      gameId: state.game_id,
      phase: phase,
      phaseVersion: version,
      deadline: Presenter.iso(deadline)
    })

    broadcast(state, "room.state", Presenter.public_state(state))
    state
  end

  defp finish_game(state) do
    counts = Enum.frequencies(Map.values(state.votes))
    wolf_id = Enum.find_value(state.roles, fn {id, role} -> if role == "WORD_WOLF", do: id end)
    wolf_votes = Map.get(counts, wolf_id, 0)
    max_votes = counts |> Map.values() |> Enum.max(fn -> 0 end)

    sole_max =
      wolf_votes > 0 and wolf_votes == max_votes and
        Enum.count(counts, fn {_id, count} -> count == max_votes end) == 1

    if sole_max and state.settings.comeback_enabled do
      comeback = %{gameId: state.game_id, wolfUserId: wolf_id, content: ""}
      transition(%{state | comeback: comeback}, "COMEBACK", 30_000)
    else
      publish_result(state, Presenter.game_result(state, counts, wolf_id, sole_max))
    end
  end

  defp finish_comeback(state, timed_out) do
    answer = state.comeback.content

    correct =
      not timed_out and
        normalize_answer(answer) == normalize_answer(Presenter.majority_word(state))

    state = %{state | comeback_result: %{answer: answer, correct: correct, timedOut: timed_out}}

    result =
      Presenter.game_result(
        state,
        Enum.frequencies(Map.values(state.votes)),
        state.comeback.wolfUserId,
        true
      )

    result = if correct, do: %{result | winner: "WOLF_SIDE", reason: "WOLF_GUESSED"}, else: result
    publish_result(state, result)
  end

  defp publish_result(state, result) do
    Rooms.persist_member_ready(state.room_id, state.host_user_id, true)
    state = transition(%{state | result: result}, "RESULT", result_ms())
    Games.record_result(state.game_id, result.winner, result.reason, result.comeback)
    broadcast(state, "result.published", result)
    state
  end

  defp validate_comeback(state, {user_id, _} = actor) do
    with :ok <- validate_action(state, actor),
         :ok <- require_phase(state, "COMEBACK"),
         :ok <- require_before_deadline(state) do
      if state.comeback.wolfUserId == user_id, do: :ok, else: {:error, :forbidden}
    end
  end

  defp validate_answer(content, allow_empty) when is_binary(content) do
    if String.valid?(content) and String.length(content) <= 80 and
         (allow_empty or String.trim(content) != ""), do: :ok, else: {:error, :invalid_payload}
  end

  defp validate_answer(_, _), do: {:error, :invalid_payload}

  defp draft_rate(%{comeback_updated_at: last}, now) when is_integer(last) and now - last < 100,
    do: {:error, :rate_limited}

  defp draft_rate(_, _), do: :ok

  defp normalize_answer(answer) do
    answer
    |> String.normalize(:nfkc)
    |> String.downcase()
    |> String.replace(~r/\s/u, "")
    |> String.to_charlist()
    |> Enum.map(fn c -> if c in 0x30A1..0x30F6, do: c - 0x60, else: c end)
    |> List.to_string()
  end

  defp reset_lobby(state) do
    cancel_timer(state.timer_ref)
    cancel_timer(state.reconnect_timer_ref)
    completed_game_id = state.game_id

    if completed_game_id do
      Registry.unregister(WordWolf.GameRegistry, {:game, completed_game_id})
      # 結果表示後のタイムアウト・再接続待ち切れの両方がここを通る。IN_GAMEの解除も
      # DB書き込みを伴うので、呼び出し元の応答をブロックしないよう1呼吸ずらす。
      send(self(), {:presence_game_ended, Map.keys(state.players)})
    end

    Rooms.persist_lobby(state.room_id, state.host_user_id)

    # 結果の後はホスト以外を準備中へ戻す。ホストは常に準備完了（契約 asyncapi.yaml onRoomReady）
    players =
      Map.new(state.players, fn {id, player} ->
        {id, Map.put(player, :ready, id == state.host_user_id)}
      end)

    state = %{
      state
      | players: players,
        phase: "LOBBY",
        phase_version: state.phase_version + 1,
        game_id: nil,
        started_at: nil,
        word_group: nil,
        roles: %{},
        words: %{},
        votes: %{},
        skip_votes: MapSet.new(),
        result: nil,
        comeback: nil,
        comeback_result: nil,
        comeback_updated_at: nil,
        deadline: nil,
        timer_ref: nil,
        paused: false,
        paused_remaining_ms: nil,
        reconnect_deadline: nil,
        reconnect_timer_ref: nil,
        disconnected: MapSet.new(),
        chat_rate: %{},
        seen_messages: MapSet.new()
    }

    broadcast(state, "phase.changed", %{
      gameId: completed_game_id,
      phase: "LOBBY",
      phaseVersion: state.phase_version,
      deadline: nil
    })

    broadcast(state, "room.state", Presenter.public_state(state))
    state
  end

  defp replace_connection(state, user_id) do
    case Map.get(state.connections, user_id) do
      nil ->
        state

      old ->
        send(old.pid, {:connection_replaced, old.id})
        Process.demonitor(old.monitor_ref, [:flush])
        %{state | monitors: Map.delete(state.monitors, old.monitor_ref)}
    end
  end

  defp set_connected(state, user_id, connected) do
    if Map.has_key?(state.players, user_id) do
      if state.game_id, do: Games.set_player_connected(state.game_id, user_id, connected)

      %{
        state
        | players: Map.update!(state.players, user_id, &Map.put(&1, :connected, connected))
      }
    else
      state
    end
  end

  defp send_private_state(%{game_id: nil}, _user_id), do: :ok

  defp send_private_state(state, user_id) do
    case Presenter.private_state(state, user_id) do
      {:ok, nil} -> :ok
      {:ok, payload} -> Endpoint.broadcast("user:#{user_id}", "private.state", payload)
    end
  end

  defp choose_word_group(state) do
    case RoomLoader.available_word_groups(state) do
      [] -> {:error, :not_found}
      pairs -> {:ok, Enum.at(pairs, random_index(length(pairs)))}
    end
  end

  defp ready_to_start(state) do
    count = map_size(state.players)

    cond do
      count < state.settings.min_players or count > state.settings.max_players ->
        {:error, :not_ready}

      Enum.any?(state.players, fn {_id, p} -> not p.ready or not p.connected end) ->
        {:error, :not_ready}

      true ->
        :ok
    end
  end

  defp validate_action(state, actor),
    do: with(:ok <- validate_connection(state, actor), do: require_not_paused(state))

  defp validate_connection(state, {user_id, connection_id}),
    do:
      if(match?(%{id: ^connection_id}, Map.get(state.connections, user_id)),
        do: :ok,
        else: {:error, :stale_connection}
      )

  defp require_not_paused(%{paused: true}), do: {:error, :game_paused}
  defp require_not_paused(_), do: :ok
  defp require_phase(%{phase: phase}, phase), do: :ok
  defp require_phase(_, _), do: {:error, :invalid_phase}

  defp require_player(state, id),
    do: if(Map.has_key?(state.players, id), do: :ok, else: {:error, :not_a_player})

  defp require_host(%{host_user_id: id}, id), do: :ok
  defp require_host(_, _), do: {:error, :not_host}

  defp not_voted(state, id),
    do: if(Map.has_key?(state.votes, id), do: {:error, :already_submitted}, else: :ok)

  defp valid_target(state, id, target),
    do:
      if(id != target and Map.has_key?(state.players, target),
        do: :ok,
        else: {:error, :invalid_target}
      )

  defp require_before_deadline(state),
    do:
      if(state.deadline && DateTime.compare(DateTime.utc_now(), state.deadline) == :lt,
        do: :ok,
        else: {:error, :deadline_exceeded}
      )

  defp broadcast_paused(state),
    do:
      broadcast(state, "game.paused", %{
        reason: "PLAYER_DISCONNECTED",
        disconnectedUserIds: MapSet.to_list(state.disconnected),
        reconnectDeadline: Presenter.iso(state.reconnect_deadline)
      })

  defp broadcast(state, event, payload),
    do: Endpoint.broadcast("room:#{state.room_id}", event, payload)

  defp schedule(ms, version),
    do:
      {DateTime.add(DateTime.utc_now(), ms, :millisecond),
       Process.send_after(self(), {:phase_timeout, version}, ms)}

  # WordWolf.Chat のエラー語彙を Channel のエラーコードへ寄せる。
  # フェーズ・権限は上で判定済みなので、ここに来る失敗は基本的に本文検証だけ。
  defp publish_chat(state, user_id, content) do
    case Chat.publish(state.game_id, user_id, content) do
      {:ok, message} -> {:ok, message}
      {:error, reason} when reason in [:empty, :too_long] -> {:error, :invalid_payload}
      {:error, :game_not_found} -> {:error, :not_found}
      {:error, reason} -> {:error, reason}
    end
  end

  defp dealing_ms, do: Application.get_env(:word_wolf, :dealing_ms, @default_dealing_ms)
  defp result_ms, do: Application.get_env(:word_wolf, :result_ms, @default_result_ms)

  defp cancel_timer(nil), do: :ok
  defp cancel_timer(ref), do: Process.cancel_timer(ref, async: true, info: false)
  defp via(room_id), do: {:via, Registry, {WordWolf.GameRegistry, {:room, room_id}}}

  defp id(prefix),
    do: "#{prefix}_#{16 |> :crypto.strong_rand_bytes() |> Base.url_encode64(padding: false)}"

  defp random_index(size) do
    <<number::unsigned-integer-size(64)>> = :crypto.strong_rand_bytes(8)
    limit = div(Integer.pow(2, 64), size) * size

    if number < limit, do: rem(number, size), else: random_index(size)
  end
end
