defmodule WordWolf.Games.Game do
  @moduledoc false

  use Ecto.Schema

  @primary_key {:id, :string, autogenerate: false}
  @foreign_key_type :string
  @timestamps_opts [type: :utc_datetime_usec]

  schema "games" do
    field :phase, :string
    field :phase_version, :integer, default: 0
    field :deadline, :utc_datetime_usec
    field :paused, :boolean, default: false
    field :reconnect_deadline, :utc_datetime_usec
    field :winner, :string
    field :result_reason, :string
    field :comeback_result, :map
    field :started_at, :utc_datetime_usec
    field :ended_at, :utc_datetime_usec
    field :aborted_at, :utc_datetime_usec

    belongs_to :room, WordWolf.Rooms.Room
    belongs_to :word_group, WordWolf.Rooms.WordGroup
    has_many :players, WordWolf.Games.GamePlayer
    has_many :chat_messages, WordWolf.Games.ChatMessage
    has_many :votes, WordWolf.Games.Vote

    timestamps()
  end
end
