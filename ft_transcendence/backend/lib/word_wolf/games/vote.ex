defmodule WordWolf.Games.Vote do
  @moduledoc false

  use Ecto.Schema

  @primary_key {:id, :string, autogenerate: false}
  @foreign_key_type :string
  @timestamps_opts [type: :utc_datetime_usec]

  schema "votes" do
    field :voted_at, :utc_datetime_usec

    belongs_to :game, WordWolf.Games.Game
    belongs_to :voter_user, WordWolf.Accounts.User
    belongs_to :target_user, WordWolf.Accounts.User

    timestamps(updated_at: false)
  end
end
