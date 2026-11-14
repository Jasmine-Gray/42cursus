defmodule WordWolf.Repo do
  use Ecto.Repo,
    otp_app: :word_wolf,
    adapter: Ecto.Adapters.Postgres
end
