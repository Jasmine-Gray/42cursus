defmodule WordWolfWeb.UserControllerTest do
  use WordWolfWeb.ConnCase, async: true

  import Ecto.Query

  alias WordWolf.Accounts
  alias WordWolf.Repo
  alias WordWolf.Rooms.RoomMember

  # fixture ユーザー + セッションで API の振る舞いを検証する（c/user-avatar 由来）
  defp create_user_and_token(_context) do
    user = user_fixture(%{display_name: "テストユーザー", avatar_url: "/avatars/yu.webp"})
    {_session, token} = session_fixture(user)
    {:ok, user: user, token: token}
  end

  describe "GET /users/:userId" do
    setup :create_user_and_token

    test "別の参加者の名前・アバター・ひとことだけを返す", %{conn: conn, token: token} do
      other = user_fixture(%{display_name: "参加者", bio: "推理が好きです"})
      response = conn |> authenticate_user(token) |> get("/users/#{other.id}")

      assert json_response(response, 200)["data"] == %{
               "userId" => other.id,
               "displayName" => other.display_name,
               "avatarUrl" => other.avatar_url,
               "bio" => "推理が好きです"
             }
    end

    test "ひとこと未設定は空文字で返す", %{conn: conn, token: token} do
      user = user_fixture(%{bio: nil})
      response = conn |> authenticate_user(token) |> get("/users/#{user.id}")
      assert json_response(response, 200)["data"]["bio"] == ""
    end

    test "存在しない参加者は404", %{conn: conn, token: token} do
      response = conn |> authenticate_user(token) |> get("/users/missing")
      assert json_response(response, 404)["error"]["code"] == "NOT_FOUND"
    end

    test "未認証では取得できない", %{conn: conn, user: user} do
      assert conn |> get("/users/#{user.id}") |> json_response(401)
    end
  end

  describe "PUT /users/me/avatar" do
    setup :create_user_and_token

    test "プリセット選択（JSON）でアバター画像を変更できる", %{conn: conn, token: token, user: user} do
      conn =
        conn
        |> authenticate_user(token)
        |> put(~p"/users/me/avatar", %{presetId: "cat"})

      assert %{"status" => "ok", "data" => data} = json_response(conn, 200)
      assert data["userId"] == user.id
      assert data["avatarUrl"] == "/avatars/cat.webp"
      assert data["displayName"] == user.display_name
      assert data["email"] == user.email

      # DB上の値も更新されていることを確認
      updated_user = Accounts.get_user!(user.id)
      assert updated_user.avatar_url == "/avatars/cat.webp"
    end

    test "画像ファイル（multipart/form-data）アップロードでアバター画像を変更できる", %{
      conn: conn,
      token: token,
      user: user
    } do
      tmp_dir = System.tmp_dir!()
      file_path = Path.join(tmp_dir, "avatar_upload_#{System.unique_integer([:positive])}.png")
      File.cp!("test/fixtures/avatars/portrait.png", file_path)
      on_exit(fn -> File.rm(file_path) end)

      upload = %Plug.Upload{
        path: file_path,
        filename: "upload.png",
        content_type: "image/png"
      }

      conn =
        conn
        |> authenticate_user(token)
        |> put(~p"/users/me/avatar", %{file: upload})

      assert %{"status" => "ok", "data" => data} = json_response(conn, 200)
      assert String.starts_with?(data["avatarUrl"], "/uploads/avatars/avatar_")
      assert String.ends_with?(data["avatarUrl"], ".png")

      # 保存されたファイルの存在確認とクリーンアップ
      relative_path = String.trim_leading(data["avatarUrl"], "/")

      target_full_path =
        Path.join([to_string(:code.priv_dir(:word_wolf)), "static", relative_path])

      on_exit(fn -> File.rm(target_full_path) end)
      assert File.read!(target_full_path) == File.read!(file_path)
      assert Accounts.get_user!(user.id).avatar_url == data["avatarUrl"]

      me = conn |> recycle() |> authenticate_user(token) |> get(~p"/users/me")
      assert json_response(me, 200)["data"]["user"]["avatarUrl"] == data["avatarUrl"]

      image = conn |> recycle() |> get(data["avatarUrl"])
      assert response(image, 200) == File.read!(file_path)
    end

    test "存在しないプリセットIDを指定した場合は 400 INVALID_PAYLOAD", %{conn: conn, token: token} do
      conn =
        conn
        |> authenticate_user(token)
        |> put(~p"/users/me/avatar", %{presetId: "nonexistent_preset"})

      assert %{"status" => "error", "error" => error} = json_response(conn, 400)
      assert error["code"] == "INVALID_PAYLOAD"
    end

    test "未認証リクエストは 401 UNAUTHENTICATED", %{conn: conn} do
      conn = put(conn, ~p"/users/me/avatar", %{presetId: "cat"})

      assert %{"status" => "error", "error" => error} = json_response(conn, 401)
      assert error["code"] == "UNAUTHENTICATED"
    end
  end

  describe "GET /users/me" do
    setup :create_user_and_token

    test "認証済みユーザーのプロフィールと戦績・状態を取得できる", %{conn: conn, token: token, user: user} do
      conn =
        conn
        |> authenticate_user(token)
        |> get(~p"/users/me")

      assert %{"status" => "ok", "data" => data} = json_response(conn, 200)

      assert %{
               "user" => user_data,
               "stats" => stats,
               "friendCount" => friend_count,
               "activeRoom" => active_room
             } = data

      assert user_data["userId"] == user.id
      assert user_data["displayName"] == user.display_name
      assert user_data["avatarUrl"] == user.avatar_url
      # 新規ユーザーは戦績ゼロ・フレンドなし・参加中の部屋なし
      assert stats == %{"gamesPlayed" => 0, "wins" => 0, "losses" => 0}
      assert friend_count == 0
      assert active_room == nil
    end

    test "未認証リクエストは 401 UNAUTHENTICATED", %{conn: conn} do
      conn = get(conn, ~p"/users/me")

      assert %{"status" => "error", "error" => error} = json_response(conn, 401)
      assert error["code"] == "UNAUTHENTICATED"
    end

    test "期限切れ・偽のトークンは 401 UNAUTHENTICATED", %{conn: conn} do
      conn = conn |> authenticate_user("not-a-real-token") |> get(~p"/users/me")

      assert %{"error" => %{"code" => "UNAUTHENTICATED"}} = json_response(conn, 401)
    end
  end

  describe "PUT /users/me" do
    setup :create_user_and_token

    test "表示名と自己紹介を更新できる", %{conn: conn, token: token, user: user} do
      conn =
        conn
        |> authenticate_user(token)
        |> put(~p"/users/me", %{displayName: "探偵ユウ", bio: "推理はお任せあれ"})

      assert %{"status" => "ok", "data" => data} = json_response(conn, 200)
      assert data["userId"] == user.id
      assert data["displayName"] == "探偵ユウ"
      assert data["bio"] == "推理はお任せあれ"
    end

    test "表示名が1文字の場合は 400 INVALID_PAYLOAD", %{conn: conn, token: token} do
      conn =
        conn
        |> authenticate_user(token)
        |> put(~p"/users/me", %{displayName: "A"})

      assert %{"status" => "error", "error" => error} = json_response(conn, 400)
      assert error["code"] == "INVALID_PAYLOAD"
      assert error["details"]["display_name"] != nil
    end

    test "未認証リクエストは 401 UNAUTHENTICATED", %{conn: conn} do
      conn = put(conn, ~p"/users/me", %{displayName: "探偵ユウ"})

      assert %{"status" => "error", "error" => error} = json_response(conn, 401)
      assert error["code"] == "UNAUTHENTICATED"
    end
  end

  # 開発シード（WordWolf.DevSeeds）の 田中 ユウ で、MeData の中身が DB から正しく組み立てられることを
  # 検証する（d/home-page 由来）。シードのセッション token は dev-token-yu。
  describe "GET /users/me（開発シード 田中 ユウ）" do
    setup %{conn: conn} do
      WordWolf.DevSeeds.run()
      {:ok, conn: authenticate_user(conn, "dev-token-yu")}
    end

    test "封筒形式でシードの田中ユウの MeData を返す", %{conn: conn} do
      conn = get(conn, ~p"/users/me")

      assert %{"status" => "ok", "data" => data} = json_response(conn, 200)

      assert %{
               "user" => user,
               "stats" => stats,
               "friendCount" => friend_count,
               "activeRoom" => active_room
             } = data

      # user は UserPrivate（userId / displayName / avatarUrl / email は必須）
      assert user["userId"] == "user_yu"
      assert user["displayName"] == "田中 ユウ"
      assert user["avatarUrl"] == "/avatars/yu.webp"
      assert user["email"] == "player@example.com"
      assert user["bio"] == "推理ゲームが好きです"

      # stats は決着済み3戦（2勝1敗）。勝率は持たない（クライアント導出）。
      assert stats == %{"gamesPlayed" => 3, "wins" => 2, "losses" => 1}
      refute Map.has_key?(stats, "winRate")

      # フレンドは ACCEPTED 3件（PENDING は数えない）
      assert friend_count == 3

      # activeRoom は CASE-042（WAITING、プレイヤー6・観戦1）。契約 RoomPreview の必須キーを全部持つ
      assert active_room["inviteCode"] == "CASE-042"
      assert active_room["roomName"] == "深夜の読書会"
      assert active_room["status"] == "WAITING"
      assert active_room["genres"] == ["PLACE", "HOBBY"]
      assert active_room["playerCount"] == 6
      assert active_room["spectatorCount"] == 1
      assert active_room["maxPlayers"] == 8
      assert active_room["joinable"] == true
    end

    test "activeRoom は参加中の部屋が無ければ null", %{conn: conn} do
      Repo.delete_all(from(m in RoomMember, where: m.user_id == "user_yu"))

      conn = get(conn, ~p"/users/me")

      assert %{"data" => %{"activeRoom" => nil}} = json_response(conn, 200)
    end
  end
end
