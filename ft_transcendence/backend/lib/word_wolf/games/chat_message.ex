defmodule WordWolf.Games.ChatMessage do
  @moduledoc false

  use Ecto.Schema

  @primary_key {:id, :string, autogenerate: false}
  @foreign_key_type :string
  @timestamps_opts [type: :utc_datetime_usec]

  schema "chat_messages" do
    field :content, :string
    field :sent_at, :utc_datetime_usec

    belongs_to :game, WordWolf.Games.Game
    belongs_to :user, WordWolf.Accounts.User

    timestamps(updated_at: false)
  end
end
