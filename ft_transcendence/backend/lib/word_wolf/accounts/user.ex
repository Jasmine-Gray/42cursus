defmodule WordWolf.Accounts.User do
  @moduledoc """
  ユーザー（`users`）。パスワードは `password_hash`（bcrypt）だけを保存し、
  仮想フィールド `password` は登録 changeset の入力にのみ使う。
  """

  use Ecto.Schema
  import Ecto.Changeset

  @primary_key {:id, :string, autogenerate: false}
  @foreign_key_type :string
  @timestamps_opts [type: :utc_datetime_usec]

  schema "users" do
    field :email, :string
    field :password_hash, :string
    field :password, :string, virtual: true
    field :display_name, :string
    field :avatar_url, :string, default: "/avatars/owl.webp"
    field :bio, :string
    field :presence_state, :string, default: "OFFLINE"
    field :last_seen_at, :utc_datetime_usec
    field :ws_connection_count, :integer, default: 0
    field :terms_accepted_at, :utc_datetime_usec

    has_many :sessions, WordWolf.Accounts.Session
    has_many :room_memberships, WordWolf.Rooms.RoomMember
    has_many :game_participations, WordWolf.Games.GamePlayer

    timestamps()
  end

  @type t :: %__MODULE__{}

  @doc "新規登録（POST /auth/register）。契約 RegisterRequest の検証をここで行う。"
  @spec registration_changeset(t(), map()) :: Ecto.Changeset.t()
  def registration_changeset(user, attrs) do
    user
    |> cast(attrs, [:email, :password, :display_name])
    |> validate_required([:email, :password, :display_name])
    |> validate_format(:email, ~r/^[^\s]+@[^\s]+$/, message: "有効なメールアドレスを入力してください")
    |> validate_length(:password, min: 8)
    |> validate_length(:display_name, min: 2, max: 24)
    |> unique_constraint(:email)
    |> put_terms_accepted(attrs)
    |> put_password_hash()
  end

  @doc "アバター変更（PUT /users/me/avatar）。"
  @spec avatar_changeset(t(), map()) :: Ecto.Changeset.t()
  def avatar_changeset(user, attrs) do
    user
    |> cast(attrs, [:avatar_url])
    |> validate_required([:avatar_url])
  end

  @doc "プロフィール編集（PUT /users/me）。契約 UpdateProfileRequest の長さ制限。"
  @spec profile_changeset(t(), map()) :: Ecto.Changeset.t()
  def profile_changeset(user, attrs) do
    user
    |> cast(attrs, [:display_name, :bio])
    |> validate_length(:display_name, min: 2, max: 24)
    |> validate_length(:bio, max: 80)
  end

  defp put_terms_accepted(changeset, %{terms_accepted: true}) do
    put_change(changeset, :terms_accepted_at, DateTime.utc_now())
  end

  defp put_terms_accepted(changeset, _attrs) do
    add_error(changeset, :terms_accepted, "利用規約への同意が必要です")
  end

  defp put_password_hash(
         %Ecto.Changeset{valid?: true, changes: %{password: password}} = changeset
       ) do
    put_change(changeset, :password_hash, Bcrypt.hash_pwd_salt(password))
  end

  defp put_password_hash(changeset), do: changeset
end
