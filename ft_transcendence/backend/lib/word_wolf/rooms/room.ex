defmodule WordWolf.Rooms.Room do
  @moduledoc false

  use Ecto.Schema
  import Ecto.Changeset

  @primary_key {:id, :string, autogenerate: false}
  @foreign_key_type :string
  @timestamps_opts [type: :utc_datetime_usec]

  schema "rooms" do
    field :room_name, :string
    field :invite_code, :string
    field :status, :string, default: "WAITING"
    field :genres, {:array, :string}
    field :comeback_enabled, :boolean, default: false
    field :discussion_ms, :integer, default: 300_000
    field :vote_ms, :integer, default: 30_000
    field :min_players, :integer, default: 3
    field :max_players, :integer, default: 8
    field :reconnect_grace_ms, :integer, default: 20_000
    field :closed_at, :utc_datetime_usec

    belongs_to :host_user, WordWolf.Accounts.User
    has_many :members, WordWolf.Rooms.RoomMember
    has_many :games, WordWolf.Games.Game

    timestamps()
  end

  @doc false
  def changeset(room, attrs) do
    room
    |> cast(attrs, [
      :room_name,
      :invite_code,
      :status,
      :genres,
      :comeback_enabled,
      :discussion_ms,
      :vote_ms,
      :host_user_id,
      :closed_at
    ])
    |> validate_inclusion(:status, ["WAITING", "IN_GAME", "CLOSED"])
  end
end
