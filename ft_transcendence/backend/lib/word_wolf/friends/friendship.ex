defmodule WordWolf.Friends.Friendship do
  @moduledoc false

  use Ecto.Schema

  @primary_key {:id, :string, autogenerate: false}
  @foreign_key_type :string
  @timestamps_opts [type: :utc_datetime_usec]

  schema "friendships" do
    field :status, :string, default: "PENDING"
    field :responded_at, :utc_datetime_usec

    belongs_to :requester, WordWolf.Accounts.User
    belongs_to :addressee, WordWolf.Accounts.User

    timestamps()
  end
end
