defmodule WordWolf.Rooms.RoomInvite do
  @moduledoc false

  use Ecto.Schema
  import Ecto.Changeset

  @primary_key {:id, :string, autogenerate: false}
  @foreign_key_type :string
  @timestamps_opts [type: :utc_datetime_usec]

  schema "room_invites" do
    field :status, :string, default: "PENDING"
    field :sent_at, :utc_datetime_usec
    field :responded_at, :utc_datetime_usec

    belongs_to :room, WordWolf.Rooms.Room
    belongs_to :inviter, WordWolf.Accounts.User
    belongs_to :invitee, WordWolf.Accounts.User

    timestamps()
  end

  @doc false
  def changeset(room_invite, attrs) do
    room_invite
    |> cast(attrs, [:id, :room_id, :inviter_id, :invitee_id, :status, :sent_at, :responded_at])
    |> validate_required([:id, :room_id, :inviter_id, :invitee_id, :status, :sent_at])
    |> validate_inclusion(:status, ["PENDING", "ACCEPTED", "DECLINED"])
    |> foreign_key_constraint(:room_id)
    |> foreign_key_constraint(:inviter_id)
    |> foreign_key_constraint(:invitee_id)
  end
end
