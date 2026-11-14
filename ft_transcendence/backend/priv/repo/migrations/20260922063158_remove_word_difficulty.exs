defmodule WordWolf.Repo.Migrations.RemoveWordDifficulty do
  use Ecto.Migration

  def up do
    drop index(:word_groups, [:genre, :word_proximity, :words])
    drop constraint(:word_groups, :word_pairs_word_proximity_check)
    drop constraint(:rooms, :rooms_word_proximity_check)

    alter table(:rooms) do
      remove :word_proximity
    end

    alter table(:word_groups) do
      remove :word_proximity
    end
  end

  def down do
    alter table(:rooms) do
      add :word_proximity, :string, default: "NORMAL", null: false
    end

    alter table(:word_groups) do
      add :word_proximity, :string, default: "NORMAL", null: false
    end

    create constraint(:word_groups, :word_pairs_word_proximity_check,
             check: "word_proximity IN ('EASY', 'NORMAL', 'HARD')"
           )

    create constraint(:rooms, :rooms_word_proximity_check,
             check: "word_proximity IN ('EASY', 'NORMAL', 'HARD')"
           )

    create unique_index(:word_groups, [:genre, :word_proximity, :words])
  end
end
