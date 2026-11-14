defmodule WordWolf.Accounts do
  @moduledoc """
  認証・ユーザーのコンテキスト。

  - 登録・ログイン・セッション発行（`/auth/*`）
  - Bearer トークンからのセッション検証（`WordWolfWeb.Plugs.Authenticate` が呼ぶ）
  - プロフィール・アバターの更新と、契約 `UserPrivate` への変換
  - ユーザー検索（`GET /users`）

  セッショントークンは生の値を保存せず、`hash_token/1`（SHA-256）で保存・検索する。
  """

  import Ecto.Query

  alias WordWolf.Accounts.{AvatarImage, Session, User}
  alias WordWolf.Contract.Types
  alias WordWolf.Repo

  @session_ttl_seconds 60 * 60 * 24 * 14

  @avatar_presets ~w(
    child-detective
    teen-detective
    yu
    minato
    aoi
    ren
    yuna
    kei
    midlife-detective
    senior-detective
    cat
    shiba
    rabbit
    owl
  )

  # --- 登録・ログイン（POST /auth/register, /auth/login） ---

  @doc """
  新規登録。メール重複は `{:error, :email_taken}`（契約 409 EMAIL_TAKEN）、
  それ以外の検証エラーは `{:error, changeset}`（400 INVALID_PAYLOAD）。
  """
  @spec register_user(map()) :: {:ok, User.t()} | {:error, :email_taken | Ecto.Changeset.t()}
  def register_user(attrs) do
    %User{id: generate_id("user")}
    |> User.registration_changeset(attrs)
    |> Repo.insert()
    |> case do
      {:ok, user} ->
        {:ok, user}

      {:error, %Ecto.Changeset{} = changeset} ->
        if email_taken?(changeset), do: {:error, :email_taken}, else: {:error, changeset}
    end
  end

  @doc """
  メールとパスワードで認証する。メール・パスワードのどちらが誤りかは区別しない（契約どおり）。
  """
  @spec authenticate_user(String.t(), String.t()) ::
          {:ok, User.t()} | {:error, :invalid_credentials}
  def authenticate_user(email, password) when is_binary(email) and is_binary(password) do
    case Repo.get_by(User, email: email) do
      nil ->
        Bcrypt.no_user_verify()
        {:error, :invalid_credentials}

      %User{password_hash: password_hash} = user ->
        if valid_password?(password, password_hash) do
          {:ok, user}
        else
          {:error, :invalid_credentials}
        end
    end
  end

  def authenticate_user(_email, _password), do: {:error, :invalid_credentials}

  @doc """
  セッションを発行し、`{session, 生トークン}` を返す。生トークンは DB に保存しない。
  """
  @spec create_session(User.t()) ::
          {:ok, {Session.t(), String.t()}} | {:error, Ecto.Changeset.t()}
  def create_session(%User{} = user) do
    raw_token = generate_token()
    now = DateTime.utc_now()

    %Session{id: generate_id("sess")}
    |> Session.changeset(%{
      user_id: user.id,
      token_hash: hash_token(raw_token),
      expires_at: DateTime.add(now, @session_ttl_seconds, :second),
      last_used_at: now
    })
    |> Repo.insert()
    |> case do
      {:ok, session} -> {:ok, {session, raw_token}}
      {:error, changeset} -> {:error, changeset}
    end
  end

  @doc """
  セッションを失効させる（`POST /auth/logout`）。`delete_all` なので既に消えていても `:ok`（冪等）。
  以降そのトークンでの `get_session_by_token/1` は `{:error, :unauthenticated}` になる。
  """
  @spec delete_session(Session.t()) :: :ok
  def delete_session(%Session{id: id}) do
    from(s in Session, where: s.id == ^id) |> Repo.delete_all()
    :ok
  end

  # --- セッション検証（Plugs.Authenticate） ---

  @doc """
  トークン文字列から有効なセッションを取得し、関連するユーザーを読み込む。
  """
  @spec get_session_by_token(String.t()) :: {:ok, Session.t()} | {:error, :unauthenticated}
  def get_session_by_token(token) when is_binary(token) do
    now = DateTime.utc_now()
    hashed = hash_token(token)

    query =
      from s in Session,
        join: u in assoc(s, :user),
        where: s.token_hash == ^hashed and s.expires_at > ^now,
        preload: [user: u]

    case Repo.one(query) do
      nil ->
        {:error, :unauthenticated}

      session ->
        _ = touch_session(session, now)
        {:ok, session}
    end
  end

  def get_session_by_token(_), do: {:error, :unauthenticated}

  @doc """
  IDでユーザーを取得する。存在しない場合は nil。
  """
  @spec get_user(String.t()) :: User.t() | nil
  def get_user(id) when is_binary(id), do: Repo.get(User, id)
  def get_user(_), do: nil

  @doc """
  IDでユーザーを取得する。存在しない場合は例外を投げる。
  """
  @spec get_user!(String.t()) :: User.t()
  def get_user!(id) when is_binary(id), do: Repo.get!(User, id)

  @doc """
  表示名の部分一致でユーザーを検索する（自分自身は除外）。
  """
  @spec search_users(String.t(), String.t()) :: [User.t()]
  def search_users(current_user_id, query) when is_binary(query) do
    clean_query = String.trim(query)

    if String.length(clean_query) >= 2 do
      pattern = "%#{clean_query}%"

      User
      |> where([u], u.id != ^current_user_id)
      |> where([u], ilike(u.display_name, ^pattern))
      |> order_by([u], asc: u.display_name)
      |> Repo.all()
    else
      []
    end
  end

  def search_users(_current_user_id, _query), do: []

  # --- プロフィール・アバター（PUT /users/me, PUT /users/me/avatar） ---

  @doc """
  アバターを更新する（プリセット選択または画像アップロード）。
  """
  @spec update_user_avatar(User.t(), map()) ::
          {:ok, User.t()} | {:error, :invalid_payload | Ecto.Changeset.t()}
  def update_user_avatar(%User{} = user, %{"presetId" => preset_id}) when is_binary(preset_id) do
    apply_preset_avatar(user, preset_id)
  end

  def update_user_avatar(%User{} = user, %{"preset_id" => preset_id}) when is_binary(preset_id) do
    apply_preset_avatar(user, preset_id)
  end

  def update_user_avatar(%User{} = user, %{"file" => %Plug.Upload{} = upload}) do
    apply_uploaded_avatar(user, upload)
  end

  def update_user_avatar(_user, _params), do: {:error, :invalid_payload}

  @doc """
  プロフィール（表示名・ひとこと）を更新する。
  """
  @spec update_user_profile(User.t(), map()) ::
          {:ok, User.t()} | {:error, Ecto.Changeset.t() | :invalid_payload}
  def update_user_profile(%User{} = user, attrs) when is_map(attrs) do
    params = %{
      display_name:
        Map.get(attrs, "displayName") || Map.get(attrs, "display_name") || user.display_name,
      bio: if(Map.has_key?(attrs, "bio"), do: Map.get(attrs, "bio"), else: user.bio)
    }

    user
    |> User.profile_changeset(params)
    |> Repo.update()
  end

  def update_user_profile(_user, _attrs), do: {:error, :invalid_payload}

  @doc """
  アカウントを削除する（DELETE /users/me, deleteAccount）。取り消しできない。

  プロフィール・フレンド・ゲーム履歴・セッションは、マイグレーションで張った
  外部キーの on_delete: :delete_all が連鎖削除する（priv/repo/migrations）。
  """
  @spec delete_user(User.t()) :: :ok
  def delete_user(%User{} = user) do
    Repo.delete!(user)
    :ok
  end

  @doc "参加者の公開プロフィールを取得する。"
  @spec get_user_profile(String.t()) :: {:ok, Types.user_profile()} | {:error, :not_found}
  def get_user_profile(user_id) do
    case get_user(user_id) do
      nil -> {:error, :not_found}
      user -> {:ok, Map.put(to_user_public(user), :bio, user.bio || "")}
    end
  end

  @doc """
  User 構造体を契約の UserPublic 型のマップに変換する。
  """
  @spec to_user_public(User.t()) :: Types.user_public()
  def to_user_public(%User{} = user) do
    %{
      userId: user.id,
      displayName: user.display_name,
      avatarUrl: user.avatar_url
    }
  end

  @doc """
  User 構造体を契約の UserPrivate 型のマップに変換する。
  """
  @spec to_user_private(User.t()) :: Types.user_private()
  def to_user_private(%User{} = user) do
    base = %{
      userId: user.id,
      displayName: user.display_name,
      avatarUrl: user.avatar_url,
      email: user.email
    }

    base = if user.bio, do: Map.put(base, :bio, user.bio), else: base

    if user.inserted_at do
      Map.put(base, :createdAt, DateTime.to_iso8601(user.inserted_at))
    else
      base
    end
  end

  @doc """
  IDを生成する。
  """
  @spec generate_id(String.t()) :: String.t()
  def generate_id(prefix), do: prefix <> "_" <> random_string(12)

  @doc """
  トークン文字列の SHA-256 ハッシュ（Base64エンコード）を計算する。
  セッション発行（create_session/1）・検証（get_session_by_token/1）・開発シードで共用する。
  """
  @spec hash_token(String.t()) :: String.t()
  def hash_token(token) when is_binary(token) do
    :crypto.hash(:sha256, token) |> Base.encode64()
  end

  # --- private helpers ---

  defp email_taken?(%Ecto.Changeset{errors: errors}) do
    Enum.any?(errors, fn
      {:email, {_msg, opts}} -> opts[:constraint] == :unique
      _ -> false
    end)
  end

  # bcrypt 形式（"$2..."）以外のハッシュ（開発シードやテスト fixture のプレースホルダ）は
  # 検証せず不一致扱いにする。Bcrypt.verify_pass は不正な形式で例外を投げるため。
  defp valid_password?(password, "$2" <> _ = password_hash) do
    Bcrypt.verify_pass(password, password_hash)
  end

  defp valid_password?(_password, _password_hash) do
    Bcrypt.no_user_verify()
    false
  end

  defp generate_token, do: random_string(32)

  defp random_string(length) do
    :crypto.strong_rand_bytes(length)
    |> Base.url_encode64(padding: false)
    |> binary_part(0, length)
  end

  defp apply_preset_avatar(user, raw_preset_id) do
    clean_id = clean_preset_id(raw_preset_id)

    if clean_id in @avatar_presets do
      avatar_url = "/avatars/#{clean_id}.webp"

      user
      |> User.avatar_changeset(%{avatar_url: avatar_url})
      |> Repo.update()
    else
      {:error, :invalid_payload}
    end
  end

  defp apply_uploaded_avatar(user, %Plug.Upload{} = upload) do
    with {:ok, avatar_url, target_path} <- AvatarImage.store(upload) do
      case user |> User.avatar_changeset(%{avatar_url: avatar_url}) |> Repo.update() do
        {:ok, updated_user} ->
          {:ok, updated_user}

        {:error, changeset} ->
          File.rm(target_path)
          {:error, changeset}
      end
    end
  end

  defp clean_preset_id(raw_id) do
    raw_id
    |> String.replace_prefix("/avatars/", "")
    |> String.replace_suffix(".webp", "")
  end

  defp touch_session(session, now) do
    session
    |> Ecto.Changeset.change(%{last_used_at: now})
    |> Repo.update()
  end
end
