defmodule WordWolf.Games.GamePlayer do
  @moduledoc false

  use Ecto.Schema

  @primary_key {:id, :string, autogenerate: false}
  @foreign_key_type :string
  @timestamps_opts [type: :utc_datetime_usec]

  schema "game_players" do
    field :participant_type, :string
    field :role, :string
    field :assigned_word, :string
    field :connected, :boolean, default: true

    belongs_to :game, WordWolf.Games.Game
    belongs_to :user, WordWolf.Accounts.User

    timestamps()
  end
end
