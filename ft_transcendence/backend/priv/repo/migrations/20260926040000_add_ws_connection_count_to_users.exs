defmodule WordWolf.Repo.Migrations.AddWsConnectionCountToUsers do
  use Ecto.Migration

  def change do
    alter table(:users) do
      add :ws_connection_count, :integer, null: false, default: 0
    end

    create constraint(:users, :users_ws_connection_count_non_negative,
             check: "ws_connection_count >= 0"
           )
  end
end
