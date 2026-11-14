defmodule WordWolfWeb.UserChannel do
  @moduledoc false
  use WordWolfWeb, :channel

  alias WordWolf.Games.{GameServer, GameSupervisor}
  alias WordWolf.Presence

  @impl true
  def join("user:" <> user_id, %{"roomId" => room_id, "connectionId" => connection_id}, socket)
      when is_binary(room_id) and is_binary(connection_id) do
    with true <- own_topic?(socket, user_id),
         {:ok, server} <- GameSupervisor.ensure_started(room_id),
         true <- GameServer.valid_connection?(server, {user_id, connection_id}) do
      send(self(), :send_private_state)
      Presence.connect(user_id)

      {:ok,
       socket
       |> assign(:game_server, server)
       |> assign(:connection_id, connection_id)}
    else
      _ -> forbidden()
    end
  end

  # 部屋の文脈なし。本人確認だけで通し、招待とプレゼンスを受け取れるようにする。
  def join("user:" <> user_id, _payload, socket) do
    if own_topic?(socket, user_id) do
      Presence.connect(user_id)
      {:ok, assign(socket, :game_server, nil)}
    else
      forbidden()
    end
  end

  # join を通ったソケットが切れたときだけ呼ばれる（join 前に弾いたものは対象外）。
  # 複数タブは users.ws_connection_count の増減で数えるので、1本でも残っていれば
  # OFFLINE にはならない（WordWolf.Presence.disconnect/1）。
  @impl true
  def terminate(_reason, socket) do
    if user_id = socket.assigns[:user_id], do: Presence.disconnect(user_id)
    :ok
  end

  @impl true
  def handle_info(:send_private_state, socket) do
    actor = {socket.assigns.user_id, socket.assigns.connection_id}

    case GameServer.private_state(socket.assigns.game_server, actor) do
      {:ok, nil} -> :ok
      {:ok, payload} -> push(socket, "private.state", payload)
      {:error, _reason} -> :ok
    end

    {:noreply, socket}
  end

  defp own_topic?(socket, user_id), do: socket.assigns[:user_id] == user_id

  defp forbidden do
    {:error, %{status: "error", error: %{code: "FORBIDDEN", message: "Forbidden."}}}
  end
end
