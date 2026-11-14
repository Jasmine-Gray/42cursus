defmodule WordWolfWeb.RoomChannel do
  @moduledoc false
  use WordWolfWeb, :channel

  alias WordWolf.Games.{GameServer, GameSupervisor}

  @impl true
  def join("room:" <> room_id, _payload, socket) do
    connection_id = id("conn")

    with {:ok, pid} <- GameSupervisor.ensure_started(room_id),
         {:ok, public_state} <-
           GameServer.connect(pid, socket.assigns.user_id, connection_id, self()) do
      socket =
        socket
        |> assign(:game_server, pid)
        |> assign(:room_id, room_id)
        |> assign(:connection_id, connection_id)

      {:ok, %{status: "ok", connectionId: connection_id, publicState: public_state}, socket}
    else
      {:error, reason} -> {:error, error(reason)}
    end
  end

  @impl true
  def handle_in("room.ready", %{"ready" => ready}, socket) when is_boolean(ready) do
    channel_reply(GameServer.set_ready(server(socket), actor(socket), ready), socket)
  end

  def handle_in("room.ready", _payload, socket),
    do: {:reply, {:error, error(:invalid_payload)}, socket}

  def handle_in("discussion.skip", %{"skip" => skip}, socket) when is_boolean(skip) do
    channel_reply(GameServer.set_skip(server(socket), actor(socket), skip), socket)
  end

  def handle_in("discussion.skip", _payload, socket),
    do: {:reply, {:error, error(:invalid_payload)}, socket}

  def handle_in("chat.send", %{"clientMessageId" => client_id, "content" => content}, socket) do
    channel_reply(GameServer.send_chat(server(socket), actor(socket), client_id, content), socket)
  end

  def handle_in("chat.send", _payload, socket),
    do: {:reply, {:error, error(:invalid_payload)}, socket}

  def handle_in("comeback.draft", %{"content" => content} = payload, socket)
      when is_binary(content) and map_size(payload) == 1 do
    channel_reply(GameServer.comeback_draft(server(socket), actor(socket), content), socket)
  end

  def handle_in("comeback.draft", _, socket),
    do: {:reply, {:error, error(:invalid_payload)}, socket}

  def handle_in("comeback.submit", %{"content" => content} = payload, socket)
      when is_binary(content) and map_size(payload) == 1 do
    channel_reply(GameServer.comeback_submit(server(socket), actor(socket), content), socket)
  end

  def handle_in("comeback.submit", _, socket),
    do: {:reply, {:error, error(:invalid_payload)}, socket}

  def handle_in("state.request", payload, socket) when map_size(payload) == 0 do
    case GameServer.state(server(socket), actor(socket)) do
      {:ok, state} -> {:reply, {:ok, %{status: "ok", publicState: state}}, socket}
      {:error, reason} -> {:reply, {:error, error(reason)}, socket}
    end
  end

  def handle_in("state.request", _payload, socket),
    do: {:reply, {:error, error(:invalid_payload)}, socket}

  @impl true
  def handle_info({:connection_replaced, connection_id}, socket) do
    if socket.assigns.connection_id == connection_id do
      push(socket, "connection.replaced", %{reason: "NEWER_CONNECTION"})
      {:stop, :normal, socket}
    else
      {:noreply, socket}
    end
  end

  def handle_info({:room_removed, connection_id}, socket) do
    if socket.assigns.connection_id == connection_id do
      push(socket, "room.removed", %{roomId: socket.assigns.room_id, reason: "REMOVED_BY_HOST"})
      {:stop, :normal, socket}
    else
      {:noreply, socket}
    end
  end

  @impl true
  def terminate(_reason, socket) do
    if socket.assigns[:game_server] do
      GameServer.disconnect(server(socket), actor(socket))
    end

    :ok
  end

  defp actor(socket) do
    {socket.assigns.user_id, socket.assigns.connection_id}
  end

  defp server(socket), do: socket.assigns.game_server

  defp channel_reply({:ok, data}, socket),
    do: {:reply, {:ok, Map.put(data, :status, "ok")}, socket}

  defp channel_reply({:error, reason}, socket), do: {:reply, {:error, error(reason)}, socket}

  defp error(reason) do
    {code, message} =
      case reason do
        :not_found -> {"NOT_FOUND", "Room not found."}
        :forbidden -> {"FORBIDDEN", "You cannot join this room."}
        :invalid_payload -> {"INVALID_PAYLOAD", "Invalid event payload."}
        :invalid_phase -> {"INVALID_PHASE", "This action is not allowed in the current phase."}
        :not_a_player -> {"NOT_A_PLAYER", "Only players can perform this action."}
        :stale_connection -> {"STALE_CONNECTION", "This connection was replaced."}
        :game_paused -> {"GAME_PAUSED", "The game is waiting for reconnection."}
        :deadline_exceeded -> {"DEADLINE_EXCEEDED", "The answer deadline has passed."}
        :rate_limited -> {"RATE_LIMITED", "Too many messages."}
        _ -> {"INTERNAL_ERROR", "The operation could not be completed."}
      end

    %{status: "error", error: %{code: code, message: message}}
  end

  defp id(prefix) do
    suffix = 16 |> :crypto.strong_rand_bytes() |> Base.url_encode64(padding: false)
    "#{prefix}_#{suffix}"
  end
end
