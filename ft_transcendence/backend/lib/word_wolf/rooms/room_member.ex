defmodule WordWolf.Rooms.RoomMember do
  @moduledoc false

  use Ecto.Schema
  import Ecto.Changeset

  @primary_key {:id, :string, autogenerate: false}
  @foreign_key_type :string
  @timestamps_opts [type: :utc_datetime_usec]

  schema "room_members" do
    field :participant_type, :string
    field :ready, :boolean, default: false
    field :connected, :boolean, default: true
    field :joined_at, :utc_datetime_usec

    belongs_to :room, WordWolf.Rooms.Room
    belongs_to :user, WordWolf.Accounts.User

    timestamps()
  end

  @doc false
  def changeset(room_member, attrs) do
    room_member
    |> cast(attrs, [:ready, :connected, :participant_type])
    |> validate_inclusion(:participant_type, ["PLAYER", "SPECTATOR"])
  end
end
