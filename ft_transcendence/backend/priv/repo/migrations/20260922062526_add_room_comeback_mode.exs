defmodule WordWolf.Repo.Migrations.AddRoomComebackMode do
  use Ecto.Migration

  def up do
    alter table(:rooms) do
      add :comeback_enabled, :boolean, null: false, default: false
    end

    alter table(:games) do
      add :comeback_result, :map
    end

    drop constraint(:games, :games_phase_check)

    create constraint(:games, :games_phase_check,
             check: "phase IN ('DEALING', 'DISCUSSION', 'VOTING', 'COMEBACK', 'RESULT')"
           )

    drop constraint(:games, :games_result_reason_check)

    create constraint(:games, :games_result_reason_check,
             check:
               "result_reason IS NULL OR result_reason IN ('WOLF_IDENTIFIED', 'WOLF_ESCAPED', 'WOLF_GUESSED')"
           )
  end

  def down do
    execute "UPDATE games SET phase = 'RESULT', winner = 'CITIZEN_SIDE', result_reason = 'WOLF_IDENTIFIED' WHERE phase = 'COMEBACK'"
    execute "UPDATE games SET result_reason = 'WOLF_ESCAPED' WHERE result_reason = 'WOLF_GUESSED'"
    drop constraint(:games, :games_phase_check)

    create constraint(:games, :games_phase_check,
             check: "phase IN ('DEALING', 'DISCUSSION', 'VOTING', 'RESULT')"
           )

    drop constraint(:games, :games_result_reason_check)

    create constraint(:games, :games_result_reason_check,
             check:
               "result_reason IS NULL OR result_reason IN ('WOLF_IDENTIFIED', 'WOLF_ESCAPED')"
           )

    alter table(:games) do
      remove :comeback_result
    end

    alter table(:rooms) do
      remove :comeback_enabled
    end
  end
end
