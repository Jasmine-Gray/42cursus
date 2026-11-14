defmodule WordWolfWeb.FallbackController do
  @moduledoc """
  コントローラーのアクションが返す `{:error, ...}` タプルを一括処理するフォールバックコントローラー。
  OpenAPI 契約（ErrorResponse スキーマ）に準拠した形式で JSON エラーを返す。
  """

  use WordWolfWeb, :controller

  def call(conn, {:error, :unauthenticated}) do
    conn
    |> put_status(:unauthorized)
    |> json(error_envelope("UNAUTHENTICATED", "Authentication required."))
  end

  def call(conn, {:error, :invalid_credentials}) do
    conn
    |> put_status(:unauthorized)
    |> json(error_envelope("INVALID_CREDENTIALS", "Invalid credentials."))
  end

  def call(conn, {:error, :forbidden}) do
    conn
    |> put_status(:forbidden)
    |> json(error_envelope("FORBIDDEN", "Forbidden."))
  end

  def call(conn, {:error, :not_found}) do
    conn
    |> put_status(:not_found)
    |> json(error_envelope("NOT_FOUND", "Resource not found."))
  end

  def call(conn, {:error, :invalid_payload}) do
    conn
    |> put_status(:bad_request)
    |> json(error_envelope("INVALID_PAYLOAD", "Invalid request payload."))
  end

  def call(conn, {:error, :bad_request}) do
    conn
    |> put_status(:bad_request)
    |> json(error_envelope("INVALID_PAYLOAD", "Invalid request payload."))
  end

  def call(conn, {:error, :invalid_payload, message}) when is_binary(message) do
    conn
    |> put_status(:bad_request)
    |> json(error_envelope("INVALID_PAYLOAD", message))
  end

  def call(conn, {:error, :email_taken}) do
    conn
    |> put_status(:conflict)
    |> json(error_envelope("EMAIL_TAKEN", "Email is already in use."))
  end

  # ゲーム進行（レーンA）。契約 openapi.yaml の ErrorCode 語彙に合わせる。
  def call(conn, {:error, :not_host}) do
    conn
    |> put_status(:forbidden)
    |> json(error_envelope("NOT_HOST", "Only the host can perform this action."))
  end

  def call(conn, {:error, :not_a_player}) do
    conn
    |> put_status(:forbidden)
    |> json(error_envelope("NOT_A_PLAYER", "Only players can perform this action."))
  end

  def call(conn, {:error, :not_ready}) do
    conn
    |> put_status(:conflict)
    |> json(error_envelope("NOT_READY", "Every player must be connected and ready."))
  end

  def call(conn, {:error, :game_in_progress}) do
    conn
    |> put_status(:conflict)
    |> json(error_envelope("GAME_IN_PROGRESS", "A game is already in progress."))
  end

  def call(conn, {:error, :invalid_phase}) do
    conn
    |> put_status(:conflict)
    |> json(error_envelope("INVALID_PHASE", "This action is not allowed in the current phase."))
  end

  def call(conn, {:error, :invalid_target}) do
    conn
    |> put_status(:bad_request)
    |> json(error_envelope("INVALID_TARGET", "Invalid vote target."))
  end

  def call(conn, {:error, :deadline_exceeded}) do
    conn
    |> put_status(:conflict)
    |> json(error_envelope("DEADLINE_EXCEEDED", "The deadline for this phase has passed."))
  end

  def call(conn, {:error, :already_submitted}) do
    conn
    |> put_status(:conflict)
    |> json(error_envelope("ALREADY_SUBMITTED", "Friend request already submitted or already friends."))
  end

  def call(conn, {:error, :already_joined}) do
    conn
    |> put_status(:conflict)
    |> json(error_envelope("ALREADY_JOINED", "User has already joined the room."))
  end

  def call(conn, {:error, :room_full}) do
    conn
    |> put_status(:conflict)
    |> json(error_envelope("ROOM_FULL", "Room is full."))
  end

  def call(conn, {:error, :conflict}) do
    conn
    |> put_status(:conflict)
    |> json(error_envelope("ALREADY_JOINED", "Conflict with current state."))
  end

  def call(conn, {:error, %Ecto.Changeset{} = changeset}) do
    details =
      Ecto.Changeset.traverse_errors(changeset, fn {msg, opts} ->
        Regex.replace(~r"%{(\w+)}", msg, fn _, key ->
          opts |> Keyword.get(String.to_existing_atom(key), key) |> to_string()
        end)
      end)

    conn
    |> put_status(:bad_request)
    |> json(%{
      status: "error",
      error: %{
        code: "INVALID_PAYLOAD",
        message: "Validation failed.",
        details: details
      }
    })
  end

  defp error_envelope(code, message) do
    %{
      status: "error",
      error: %{
        code: code,
        message: message
      }
    }
  end
end
