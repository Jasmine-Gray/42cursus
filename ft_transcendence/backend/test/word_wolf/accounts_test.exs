defmodule WordWolf.AccountsTest do
  use WordWolf.DataCase, async: true

  alias WordWolf.Accounts
  import WordWolf.AccountsFixtures

  describe "get_session_by_token/1" do
    test "有効なトークンでセッションとユーザーが取得できる" do
      user = user_fixture()
      {_session, token} = session_fixture(user)

      assert {:ok, retrieved_session} = Accounts.get_session_by_token(token)
      assert retrieved_session.user.id == user.id
      assert retrieved_session.last_used_at != nil
    end

    test "存在しないトークンは :unauthenticated エラー" do
      assert {:error, :unauthenticated} = Accounts.get_session_by_token("invalid-token")
    end

    test "期限切れトークンは :unauthenticated エラー" do
      user = user_fixture()
      token = "expired-token"
      now = DateTime.utc_now() |> DateTime.truncate(:microsecond)
      expired_at = DateTime.add(now, -3600, :second)

      _expired_session =
        %WordWolf.Accounts.Session{}
        |> Ecto.Changeset.change(%{
          id: "session_expired_#{System.unique_integer([:positive])}",
          user_id: user.id,
          token_hash: Accounts.hash_token(token),
          expires_at: expired_at,
          last_used_at: now
        })
        |> Repo.insert!()

      assert {:error, :unauthenticated} = Accounts.get_session_by_token(token)
    end
  end

  describe "delete_session/1" do
    test "セッションを失効させ、以降そのトークンでは取得できない" do
      user = user_fixture()
      {session, token} = session_fixture(user)

      assert {:ok, _} = Accounts.get_session_by_token(token)
      assert :ok = Accounts.delete_session(session)
      assert {:error, :unauthenticated} = Accounts.get_session_by_token(token)
    end

    test "既に消えているセッションでも :ok（冪等）" do
      user = user_fixture()
      {session, _token} = session_fixture(user)

      assert :ok = Accounts.delete_session(session)
      assert :ok = Accounts.delete_session(session)
    end
  end

  describe "update_user_avatar/2 (プリセット選択)" do
    test "有効なプリセットIDでアバターURLが更新される" do
      user = user_fixture(%{avatar_url: "/avatars/yu.webp"})

      assert {:ok, updated} = Accounts.update_user_avatar(user, %{"presetId" => "cat"})
      assert updated.avatar_url == "/avatars/cat.webp"

      # DBから再取得して検証
      reloaded = Accounts.get_user!(user.id)
      assert reloaded.avatar_url == "/avatars/cat.webp"
    end

    test "パス形式（/avatars/shiba.webp）のプリセット指定でも更新される" do
      user = user_fixture()

      assert {:ok, updated} =
               Accounts.update_user_avatar(user, %{"presetId" => "/avatars/shiba.webp"})

      assert updated.avatar_url == "/avatars/shiba.webp"
    end

    test "snake_case（preset_id）キーでも更新できる" do
      user = user_fixture()

      assert {:ok, updated} = Accounts.update_user_avatar(user, %{"preset_id" => "rabbit"})
      assert updated.avatar_url == "/avatars/rabbit.webp"
    end

    test "無効なプリセットIDは :invalid_payload エラー" do
      user = user_fixture()

      assert {:error, :invalid_payload} =
               Accounts.update_user_avatar(user, %{"presetId" => "invalid_preset"})
    end
  end

  describe "update_user_avatar/2 (ファイルアップロード)" do
    setup do
      tmp_dir = System.tmp_dir!()
      file_path = Path.join(tmp_dir, "test_avatar_#{System.unique_integer([:positive])}.png")
      File.cp!("test/fixtures/avatars/portrait.png", file_path)

      on_exit(fn ->
        File.rm(file_path)
      end)

      {:ok, file_path: file_path}
    end

    test "有効な画像ファイルをアップロードして保存できる", %{file_path: file_path} do
      user = user_fixture()

      upload = %Plug.Upload{
        path: file_path,
        filename: "my_avatar.png",
        content_type: "image/png"
      }

      assert {:ok, updated} = Accounts.update_user_avatar(user, %{"file" => upload})
      assert String.starts_with?(updated.avatar_url, "/uploads/avatars/avatar_")
      assert String.ends_with?(updated.avatar_url, ".png")

      # 保存されたファイルが存在することを確認
      relative_path = String.trim_leading(updated.avatar_url, "/")

      target_full_path =
        Path.join([to_string(:code.priv_dir(:word_wolf)), "static", relative_path])

      assert File.exists?(target_full_path)

      # クリーンアップ
      File.rm(target_full_path)
    end

    test "不正な形式のファイルアップロードは :invalid_payload エラー", %{file_path: file_path} do
      user = user_fixture()
      File.write!(file_path, "%PDF-1.7\ninvalid avatar")

      upload = %Plug.Upload{
        path: file_path,
        filename: "document.pdf",
        content_type: "application/pdf"
      }

      assert {:error, :invalid_payload} = Accounts.update_user_avatar(user, %{"file" => upload})
    end
  end

  describe "update_user_profile/2" do
    test "表示名とひとことの更新に成功する" do
      user = user_fixture(%{display_name: "旧名前", bio: "旧自己紹介"})

      assert {:ok, updated} =
               Accounts.update_user_profile(user, %{
                 "displayName" => "新名前",
                 "bio" => "新自己紹介"
               })

      assert updated.display_name == "新名前"
      assert updated.bio == "新自己紹介"
    end

    test "表示名が短すぎる場合はエラー" do
      user = user_fixture()

      assert {:error, changeset} =
               Accounts.update_user_profile(user, %{"displayName" => "A"})

      assert "should be at least 2 character(s)" in errors_on(changeset).display_name
    end

    test "表示名が長すぎる場合はエラー" do
      user = user_fixture()
      long_name = String.duplicate("あ", 25)

      assert {:error, changeset} =
               Accounts.update_user_profile(user, %{"displayName" => long_name})

      assert "should be at most 24 character(s)" in errors_on(changeset).display_name
    end
  end

  describe "to_user_private/1" do
    test "UserPrivate契約に適合したマップを生成する" do
      user = user_fixture(%{display_name: "田中 ユウ", avatar_url: "/avatars/cat.webp"})
      result = Accounts.to_user_private(user)

      assert result.userId == user.id
      assert result.displayName == "田中 ユウ"
      assert result.avatarUrl == "/avatars/cat.webp"
      assert result.email == user.email
      assert is_binary(result.bio)
      assert is_binary(result.createdAt)
    end
  end
end
