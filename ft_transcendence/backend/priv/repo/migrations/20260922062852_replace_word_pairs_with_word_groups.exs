defmodule WordWolf.Repo.Migrations.ReplaceWordPairsWithWordGroups do
  use Ecto.Migration

  def up do
    drop constraint(:word_pairs, :word_pairs_different_words_check)

    drop index(:word_pairs, [:genre, :word_proximity, :majority_word, :minority_word],
           name: :word_pairs_unique_pair_index
         )

    alter table(:word_pairs) do
      add :words, {:array, :string}
    end

    execute "UPDATE word_pairs SET words = ARRAY[majority_word, minority_word]"

    alter table(:word_pairs) do
      modify :words, {:array, :string}, null: false
      remove :majority_word
      remove :minority_word
    end

    rename table(:word_pairs), to: table(:word_groups)
    rename table(:games), :word_pair_id, to: :word_group_id
    create constraint(:word_groups, :word_groups_size_check, check: "cardinality(words) >= 2")
    create unique_index(:word_groups, [:genre, :word_proximity, :words])
  end

  def down do
    drop index(:word_groups, [:genre, :word_proximity, :words])
    drop constraint(:word_groups, :word_groups_size_check)
    rename table(:games), :word_group_id, to: :word_pair_id
    rename table(:word_groups), to: table(:word_pairs)

    alter table(:word_pairs) do
      add :majority_word, :string
      add :minority_word, :string
    end

    execute "UPDATE word_pairs SET majority_word = words[1], minority_word = words[2]"

    alter table(:word_pairs) do
      modify :majority_word, :string, null: false
      modify :minority_word, :string, null: false
      remove :words
    end

    create constraint(:word_pairs, :word_pairs_different_words_check,
             check: "majority_word <> minority_word"
           )

    create unique_index(:word_pairs, [:genre, :word_proximity, :majority_word, :minority_word],
             name: :word_pairs_unique_pair_index
           )
  end
end
