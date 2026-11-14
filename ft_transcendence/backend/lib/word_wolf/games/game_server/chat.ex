defmodule WordWolf.Games.GameServer.Chat do
  @moduledoc false

  alias WordWolf.Games.GameServer.State

  @spec accept(State.t(), String.t(), term(), term()) ::
          {:ok, State.t(), String.t()} | {:error, :invalid_payload | :rate_limited}
  def accept(state, user_id, client_id, content) do
    with {:ok, content} <- validate_content(content),
         :ok <- validate_client_id(client_id),
         :ok <- unique_message(state, client_id),
         {:ok, state} <- consume_rate(state, user_id) do
      {:ok, %{state | seen_messages: MapSet.put(state.seen_messages, client_id)}, content}
    end
  end

  defp validate_content(content) when is_binary(content) do
    content = String.trim(content)
    if String.length(content) in 1..300, do: {:ok, content}, else: {:error, :invalid_payload}
  end

  defp validate_content(_content), do: {:error, :invalid_payload}
  defp validate_client_id(id) when is_binary(id) and byte_size(id) > 0, do: :ok
  defp validate_client_id(_id), do: {:error, :invalid_payload}

  defp unique_message(state, id),
    do: if(MapSet.member?(state.seen_messages, id), do: {:error, :invalid_payload}, else: :ok)

  defp consume_rate(state, user_id) do
    now = System.monotonic_time(:millisecond)
    recent = state.chat_rate |> Map.get(user_id, []) |> Enum.filter(&(now - &1 < 10_000))

    cond do
      recent != [] and now - hd(recent) < 1_000 -> {:error, :rate_limited}
      length(recent) >= 5 -> {:error, :rate_limited}
      true -> {:ok, %{state | chat_rate: Map.put(state.chat_rate, user_id, [now | recent])}}
    end
  end
end
