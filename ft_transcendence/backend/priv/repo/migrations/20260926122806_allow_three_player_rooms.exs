defmodule WordWolf.Repo.Migrations.AllowThreePlayerRooms do
  use Ecto.Migration

  def up do
    drop constraint(:rooms, :rooms_player_range_check)

    alter table(:rooms) do
      modify :min_players, :integer, default: 3, null: false
    end

    execute "UPDATE rooms SET min_players = 3"

    create constraint(:rooms, :rooms_player_range_check,
             check: "min_players >= 3 AND max_players <= 8 AND min_players <= max_players"
           )
  end

  def down do
    drop constraint(:rooms, :rooms_player_range_check)
    execute "UPDATE rooms SET min_players = 4 WHERE min_players < 4"

    alter table(:rooms) do
      modify :min_players, :integer, default: 4, null: false
    end

    create constraint(:rooms, :rooms_player_range_check,
             check: "min_players >= 4 AND max_players <= 8 AND min_players <= max_players"
           )
  end
end
