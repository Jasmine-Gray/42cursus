defmodule WordWolf.Accounts.Session do
  @moduledoc """
  セッション（`sessions`）。`token_hash` には生トークンの SHA-256（Base64）を保存する。
  生トークンはログイン/登録レスポンスでクライアントに一度だけ渡す。
  """

  use Ecto.Schema
  import Ecto.Changeset

  @primary_key {:id, :string, autogenerate: false}
  @foreign_key_type :string
  @timestamps_opts [type: :utc_datetime_usec]

  schema "sessions" do
    field :token_hash, :string
    field :expires_at, :utc_datetime_usec
    field :last_used_at, :utc_datetime_usec

    belongs_to :user, WordWolf.Accounts.User

    timestamps(updated_at: false)
  end

  @type t :: %__MODULE__{}

  @spec changeset(t(), map()) :: Ecto.Changeset.t()
  def changeset(session, attrs) do
    session
    |> cast(attrs, [:user_id, :token_hash, :expires_at, :last_used_at])
    |> validate_required([:user_id, :token_hash, :expires_at])
  end
end
