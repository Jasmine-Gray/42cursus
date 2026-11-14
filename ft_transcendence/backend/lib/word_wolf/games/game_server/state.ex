defmodule WordWolf.Games.GameServer.State do
  @moduledoc false

  defstruct room_id: nil,
            host_user_id: nil,
            settings: %{},
            players: %{},
            spectators: %{},
            connections: %{},
            monitors: %{},
            phase: "LOBBY",
            phase_version: 0,
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

  @type t :: %__MODULE__{}
  @type actor :: {String.t(), String.t()}
end
