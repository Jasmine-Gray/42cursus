defmodule WordWolfWeb.GameController do
  @moduledoc """
  `games` タグの HTTP エンドポイント。いまは公開チャット履歴の取得だけを持つ。
  封筒形式は `GET /health` に合わせる。`conn.assigns.current_user` は router の
  `:authenticated` パイプラインが置く。
  """
  use WordWolfWeb, :controller

  alias WordWolf.Chat
  alias WordWolf.Games.GameServer

  action_fallback WordWolfWeb.FallbackController

  @doc """
  GET /games/{gameId} (getGame)

  公開状態を返す（議論・観戦・投票観戦・再接続復帰用）。秘密情報は含めない。
  """
  def show(conn, %{"gameId" => game_id}) do
    with {:ok, state} <- GameServer.game_public_state(game_id, conn.assigns.current_user.id) do
      json(conn, %{status: "ok", data: state})
    end
  end

  @doc """
  GET /games/{gameId}/assignment (getMyAssignment)

  本人のお題だけを返す。多数派・少数派の別（役職）は返さない。
  観戦者は NOT_A_PLAYER で拒否する。
  """
  def assignment(conn, %{"gameId" => game_id}) do
    with {:ok, payload} <- GameServer.assignment(game_id, conn.assigns.current_user.id) do
      json(conn, %{status: "ok", data: payload})
    end
  end

  @doc "GET /games/{gameId}/spectator-assignments (getSpectatorAssignments)。現在の観戦者だけへ全員のお題を返す。"
  def spectator_assignments(conn, %{"gameId" => game_id}) do
    with {:ok, payload} <- GameServer.spectator_assignments(game_id, conn.assigns.current_user.id) do
      conn |> put_resp_header("cache-control", "no-store") |> json(%{status: "ok", data: payload})
    end
  end

  @doc """
  POST /games/{gameId}/votes (castVote)

  1人1回・変更不可。投票者は認証セッションから取る（リクエストには含めない）。
  受理後の進捗は Channel の vote.progress でも配信される。
  """
  def vote(conn, %{"gameId" => game_id, "targetUserId" => target_user_id})
      when is_binary(target_user_id) do
    with {:ok, progress} <-
           GameServer.cast_vote(game_id, conn.assigns.current_user.id, target_user_id) do
      conn
      |> put_status(:created)
      |> json(%{status: "ok", data: progress})
    end
  end

  def vote(_conn, _params), do: {:error, :invalid_payload}

  @doc """
  GET /games/{gameId}/result (getGameResult)

  RESULT フェーズ以降だけ取得できる。観戦者も同じ内容を見られる（myVote は null）。
  """
  def result(conn, %{"gameId" => game_id}) do
    with {:ok, payload} <- GameServer.game_result(game_id, conn.assigns.current_user.id) do
      json(conn, %{status: "ok", data: payload})
    end
  end

  @doc """
  GET /games/{gameId}/messages (listGameMessages)

  公開チャット履歴を送信時刻の昇順で返す。入室直後・再接続時の復元用。
  参加者（プレイヤー・観戦者）だけが取得できる。存在しないゲームは 404、
  参加していないゲームは 403。
  """
  @spec messages(Plug.Conn.t(), map()) :: Plug.Conn.t()
  def messages(conn, %{"game_id" => game_id}) do
    user = conn.assigns.current_user

    with :ok <- authorize(game_id, user.id) do
      json(conn, %{status: "ok", data: %{items: Chat.list_messages(game_id)}})
    end
  end

  defp authorize(game_id, user_id) do
    cond do
      not Chat.game_exists?(game_id) -> {:error, :not_found}
      not Chat.member?(game_id, user_id) -> {:error, :forbidden}
      true -> :ok
    end
  end
end
