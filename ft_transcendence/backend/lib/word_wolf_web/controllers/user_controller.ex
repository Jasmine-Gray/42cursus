defmodule WordWolfWeb.UserController do
  @moduledoc """
  ユーザー（`/users` および `/users/me`）のエンドポイント。
  `conn.assigns.current_user` は router の `:authenticated` パイプラインが置く。
  """
  use WordWolfWeb, :controller

  alias WordWolf.{Accounts, Friends, Games, Rooms}
  alias WordWolf.Accounts.User
  alias WordWolf.Contract.Types

  action_fallback WordWolfWeb.FallbackController

  @doc """
  GET /users?q=... (searchUsers)
  表示名でユーザーを検索する（自分自身は除外）。
  """
  @spec index(Plug.Conn.t(), map()) :: Plug.Conn.t()
  def index(conn, %{"q" => q}) when is_binary(q) do
    clean_q = String.trim(q)

    if String.length(clean_q) >= 2 do
      users = Accounts.search_users(conn.assigns.current_user.id, clean_q)
      items = Enum.map(users, &Accounts.to_user_public/1)
      json(conn, %{status: "ok", data: %{items: items}})
    else
      conn
      |> put_status(:bad_request)
      |> json(%{
        status: "error",
        error: %{
          code: "INVALID_PAYLOAD",
          message: "Query 'q' must be at least 2 characters."
        }
      })
    end
  end

  def index(conn, _params) do
    conn
    |> put_status(:bad_request)
    |> json(%{
      status: "error",
      error: %{
        code: "INVALID_PAYLOAD",
        message: "Query parameter 'q' is required."
      }
    })
  end

  @doc "GET /users/{userId} (getUserProfile): 名前・アバター・ひとことを返す。"
  @spec profile(Plug.Conn.t(), map()) :: Plug.Conn.t() | {:error, :not_found}
  def profile(conn, %{"userId" => user_id}) do
    with {:ok, profile} <- Accounts.get_user_profile(user_id) do
      json(conn, %{status: "ok", data: profile})
    end
  end

  @doc """
  GET /users/me (getMe)
  認証ユーザー自身の情報、戦績、フレンド数、参加中ルームを返す。
  """
  @spec show(Plug.Conn.t(), map()) :: Plug.Conn.t()
  def show(conn, _params) do
    json(conn, %{status: "ok", data: me_data(conn.assigns.current_user)})
  end

  @doc """
  PUT /users/me (updateProfile)
  表示名やひとこと（自己紹介）を更新する。
  """
  @spec update(Plug.Conn.t(), map()) :: Plug.Conn.t()
  def update(conn, params) do
    user = conn.assigns.current_user

    with {:ok, updated_user} <- Accounts.update_user_profile(user, params) do
      json(conn, %{status: "ok", data: Accounts.to_user_private(updated_user)})
    end
  end

  @doc """
  DELETE /users/me (deleteAccount)

  プロフィール・フレンド・ゲーム履歴を消す。取り消しできない。
  """
  def delete(conn, _params) do
    :ok = Accounts.delete_user(conn.assigns.current_user)
    json(conn, %{status: "ok"})
  end

  @doc """
  GET /users/me/games (listMyGames)

  画面22 プロフィールの「最近の対戦」。決着済みで中止されていないゲームを新しい順に返す。
  """
  def games(conn, params) do
    page = int_param(params, "page", 1, 1, 1_000_000)
    limit = int_param(params, "limit", 20, 1, 50)

    json(conn, %{
      status: "ok",
      data: Games.history_for(conn.assigns.current_user.id, page, limit)
    })
  end

  # クエリ文字列は文字列で来る。契約の範囲（minimum/maximum）に丸める。
  defp int_param(params, key, default, min, max) do
    case params |> Map.get(key) |> parse_int() do
      nil -> default
      value -> value |> max(min) |> min(max)
    end
  end

  defp parse_int(value) when is_integer(value), do: value

  defp parse_int(value) when is_binary(value) do
    case Integer.parse(value) do
      {n, ""} -> n
      _ -> nil
    end
  end

  defp parse_int(_value), do: nil

  @doc """
  PUT /users/me/avatar (updateAvatar)
  プリセット選択（JSON: presetId）またはファイルアップロード（multipart: file）によりアバター画像を更新する。
  """
  @spec update_avatar(Plug.Conn.t(), map()) :: Plug.Conn.t()
  def update_avatar(conn, params) do
    user = conn.assigns.current_user

    with {:ok, updated_user} <- Accounts.update_user_avatar(user, params) do
      json(conn, %{status: "ok", data: Accounts.to_user_private(updated_user)})
    end
  end

  @spec me_data(User.t()) :: Types.me_data()
  defp me_data(%User{id: user_id} = user) do
    %{
      user: Accounts.to_user_private(user),
      stats: Games.summary_for(user_id),
      friendCount: Friends.count_accepted(user_id),
      activeRoom: Rooms.active_preview_for(user_id)
    }
  end
end
