defmodule WordWolf.Rooms.WordGroup do
  @moduledoc false

  use Ecto.Schema

  @primary_key {:id, :string, autogenerate: false}
  @timestamps_opts [type: :utc_datetime_usec]

  schema "word_groups" do
    field :genre, :string
    field :words, {:array, :string}
    field :active, :boolean, default: true

    has_many :games, WordWolf.Games.Game

    timestamps()
  end
end
