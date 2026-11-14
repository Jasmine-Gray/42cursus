defmodule WordWolfWeb.AuthControllerTest do
  use WordWolfWeb.ConnCase, async: true

  alias WordWolf.Accounts

  @register_params %{
    displayName: "田中 ユウ",
    email: "yu@example.com",
    password: "password123",
    termsAccepted: true
  }

  describe "POST /auth/register" do
    test "201 で AuthSession（token + UserPrivate）を返す", %{conn: conn} do
      conn = post(conn, ~p"/auth/register", @register_params)

      assert %{"status" => "ok", "data" => %{"token" => token, "user" => user}} =
               json_response(conn, 201)

      assert is_binary(token) and byte_size(token) > 0
      assert String.starts_with?(user["userId"], "user_")
      assert user["displayName"] == "田中 ユウ"
      assert user["email"] == "yu@example.com"
      assert user["avatarUrl"] == "/avatars/owl.webp"
      refute Map.has_key?(user, "password")
      refute Map.has_key?(user, "passwordHash")
    end

    test "発行された token で GET /users/me が通る（b→c 結合）", %{conn: conn} do
      %{"data" => %{"token" => token}} =
        conn |> post(~p"/auth/register", @register_params) |> json_response(201)

      conn =
        build_conn()
        |> authenticate_user(token)
        |> get(~p"/users/me")

      assert %{"status" => "ok", "data" => %{"user" => %{"email" => "yu@example.com"}}} =
               json_response(conn, 200)
    end

    test "利用規約に同意していないと 400 INVALID_PAYLOAD", %{conn: conn} do
      conn = post(conn, ~p"/auth/register", %{@register_params | termsAccepted: false})

      assert %{"status" => "error", "error" => %{"code" => "INVALID_PAYLOAD"}} =
               json_response(conn, 400)
    end

    test "パスワードが短いと 400 INVALID_PAYLOAD", %{conn: conn} do
      conn = post(conn, ~p"/auth/register", %{@register_params | password: "short"})

      assert %{"error" => %{"code" => "INVALID_PAYLOAD", "details" => %{"password" => _}}} =
               json_response(conn, 400)
    end

    test "メール重複は 409 EMAIL_TAKEN", %{conn: conn} do
      post(conn, ~p"/auth/register", @register_params)
      conn = post(build_conn(), ~p"/auth/register", %{@register_params | displayName: "別の人"})

      assert %{"status" => "error", "error" => %{"code" => "EMAIL_TAKEN"}} =
               json_response(conn, 409)
    end
  end

  describe "POST /auth/login" do
    setup %{conn: conn} do
      post(conn, ~p"/auth/register", @register_params)
      :ok
    end

    test "正しい資格情報で 200 と新しい token を返す", %{conn: conn} do
      conn = post(conn, ~p"/auth/login", %{email: "yu@example.com", password: "password123"})

      assert %{"status" => "ok", "data" => %{"token" => token, "user" => user}} =
               json_response(conn, 200)

      assert is_binary(token)
      assert user["email"] == "yu@example.com"
    end

    test "login の token で GET /users/me が通る（b→c 結合）", %{conn: conn} do
      %{"data" => %{"token" => token}} =
        conn
        |> post(~p"/auth/login", %{email: "yu@example.com", password: "password123"})
        |> json_response(200)

      conn = build_conn() |> authenticate_user(token) |> get(~p"/users/me")

      assert %{"data" => %{"user" => %{"displayName" => "田中 ユウ"}}} = json_response(conn, 200)
    end

    test "パスワード誤りは 401 INVALID_CREDENTIALS", %{conn: conn} do
      conn = post(conn, ~p"/auth/login", %{email: "yu@example.com", password: "wrong-password"})

      assert %{"status" => "error", "error" => %{"code" => "INVALID_CREDENTIALS"}} =
               json_response(conn, 401)
    end

    test "存在しないメールも同じ 401 INVALID_CREDENTIALS（区別しない）", %{conn: conn} do
      conn = post(conn, ~p"/auth/login", %{email: "nobody@example.com", password: "password123"})

      assert %{"error" => %{"code" => "INVALID_CREDENTIALS"}} = json_response(conn, 401)
    end

    test "bcrypt 形式でないハッシュ（シード/fixture）のユーザーは 500 にならず 401", %{conn: conn} do
      user = user_fixture(%{email: "seed@example.com", password_hash: "dev-seed-placeholder"})

      conn = post(conn, ~p"/auth/login", %{email: user.email, password: "anything"})

      assert %{"error" => %{"code" => "INVALID_CREDENTIALS"}} = json_response(conn, 401)
    end

    test "email/password が欠けていると 400 INVALID_PAYLOAD", %{conn: conn} do
      conn = post(conn, ~p"/auth/login", %{email: "yu@example.com"})

      assert %{"error" => %{"code" => "INVALID_PAYLOAD"}} = json_response(conn, 400)
    end
  end

  describe "POST /auth/logout" do
    setup %{conn: conn} do
      %{"data" => %{"token" => token}} =
        conn |> post(~p"/auth/register", @register_params) |> json_response(201)

      {:ok, token: token}
    end

    test "200 OkOnly を返し、以降その token は 401 になる", %{token: token} do
      logout_conn = build_conn() |> authenticate_user(token) |> post(~p"/auth/logout")
      assert %{"status" => "ok"} = json_response(logout_conn, 200)

      me_conn = build_conn() |> authenticate_user(token) |> get(~p"/users/me")
      assert %{"error" => %{"code" => "UNAUTHENTICATED"}} = json_response(me_conn, 401)
    end

    test "ログアウト済みの token で再度ログアウトすると 401（セッションは既に消えている）", %{token: token} do
      assert build_conn() |> authenticate_user(token) |> post(~p"/auth/logout") |> json_response(200)

      assert build_conn()
             |> authenticate_user(token)
             |> post(~p"/auth/logout")
             |> json_response(401)
    end

    test "未認証は 401 UNAUTHENTICATED", %{conn: conn} do
      assert %{"error" => %{"code" => "UNAUTHENTICATED"}} =
               conn |> post(~p"/auth/logout") |> json_response(401)
    end

    test "他ユーザーのセッションには影響しない", %{token: token} do
      %{"data" => %{"token" => other_token}} =
        build_conn()
        |> post(~p"/auth/register", %{@register_params | email: "other@example.com", displayName: "別の人"})
        |> json_response(201)

      build_conn() |> authenticate_user(token) |> post(~p"/auth/logout") |> json_response(200)

      me_conn = build_conn() |> authenticate_user(other_token) |> get(~p"/users/me")
      assert %{"data" => %{"user" => %{"email" => "other@example.com"}}} = json_response(me_conn, 200)
    end
  end

  describe "開発シードのユーザー（d/home-page の DevSeeds）" do
    setup do
      WordWolf.DevSeeds.run()
      :ok
    end

    test "全シードユーザーに dev-token-<name> のセッションがあり、/users/me が通る（/dev/as/<name> の前提）", %{conn: conn} do
      users = WordWolf.Repo.all(WordWolf.Accounts.User)
      assert length(users) >= 12

      for %{id: "user_" <> suffix = id} <- users do
        conn = build_conn() |> authenticate_user("dev-token-#{suffix}") |> get(~p"/users/me")
        assert %{"data" => %{"user" => %{"userId" => ^id}}} = json_response(conn, 200)
      end

      _ = conn
    end

    test "田中 ユウ は共通の開発用パスワードでログインでき、その token で /users/me が通る", %{conn: conn} do
      conn =
        post(conn, ~p"/auth/login", %{
          email: "player@example.com",
          password: WordWolf.DevSeeds.dev_password()
        })

      assert %{"data" => %{"token" => token, "user" => %{"userId" => "user_yu"}}} =
               json_response(conn, 200)

      conn = build_conn() |> authenticate_user(token) |> get(~p"/users/me")
      assert %{"data" => %{"stats" => %{"gamesPlayed" => 3}}} = json_response(conn, 200)
    end
  end

  describe "Accounts.create_session/1 と get_session_by_token/1" do
    test "発行した生トークンで検索でき、DB には SHA-256 ハッシュだけが保存される" do
      {:ok, user} =
        Accounts.register_user(%{
          email: "hash@example.com",
          password: "password123",
          display_name: "ハッシュ確認",
          terms_accepted: true
        })

      assert {:ok, {session, raw_token}} = Accounts.create_session(user)
      assert session.token_hash == Accounts.hash_token(raw_token)
      refute session.token_hash == raw_token
      assert {:ok, %{user: %{id: user_id}}} = Accounts.get_session_by_token(raw_token)
      assert user_id == user.id
    end
  end
end
