defmodule WordWolf.Games.GameSupervisor do
  @moduledoc false

  alias WordWolf.Games.GameServer

  @spec child_spec(term()) :: Supervisor.child_spec()
  def child_spec(_arg) do
    Supervisor.child_spec({DynamicSupervisor, name: __MODULE__, strategy: :one_for_one},
      id: __MODULE__
    )
  end

  @spec ensure_started(String.t()) :: {:ok, pid()} | {:error, term()}
  def ensure_started(room_id) do
    case Registry.lookup(WordWolf.GameRegistry, {:room, room_id}) do
      [{pid, _}] -> {:ok, pid}
      [] -> start_room(room_id)
    end
  end

  defp start_room(room_id) do
    case DynamicSupervisor.start_child(__MODULE__, {GameServer, room_id}) do
      {:ok, pid} -> {:ok, pid}
      {:error, {:already_started, pid}} -> {:ok, pid}
      {:error, reason} -> {:error, reason}
    end
  end
end
