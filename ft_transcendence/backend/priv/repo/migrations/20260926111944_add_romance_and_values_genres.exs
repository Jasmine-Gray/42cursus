defmodule WordWolf.Repo.Migrations.AddRomanceAndValuesGenres do
  use Ecto.Migration

  def up do
    drop constraint(:word_groups, :word_pairs_genre_check)

    create constraint(:word_groups, :word_pairs_genre_check,
             check:
               "genre IN ('PLACE', 'FOOD', 'HOBBY', 'DAILY_ITEM', 'WORK_SCHOOL', 'ENTERTAINMENT', 'ROMANCE', 'VALUES')"
           )
  end

  def down do
    drop constraint(:word_groups, :word_pairs_genre_check)

    create constraint(:word_groups, :word_pairs_genre_check,
             check:
               "genre IN ('PLACE', 'FOOD', 'HOBBY', 'DAILY_ITEM', 'WORK_SCHOOL', 'ENTERTAINMENT')"
           )
  end
end
