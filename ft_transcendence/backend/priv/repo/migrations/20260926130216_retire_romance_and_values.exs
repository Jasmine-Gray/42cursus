defmodule WordWolf.Repo.Migrations.RetireRomanceAndValues do
  use Ecto.Migration

  def up do
    execute """
    UPDATE rooms SET genres = CASE
      WHEN cardinality(array_remove(array_remove(genres, 'ROMANCE'), 'VALUES')) > 0
      THEN array_remove(array_remove(genres, 'ROMANCE'), 'VALUES')
      ELSE ARRAY['PLACE', 'FOOD', 'HOBBY', 'DAILY_ITEM', 'WORK_SCHOOL', 'ENTERTAINMENT']::varchar[]
    END
    WHERE genres && ARRAY['ROMANCE', 'VALUES']::varchar[]
    """

    execute "UPDATE word_groups SET active = false WHERE genre IN ('ROMANCE', 'VALUES')"
  end

  # 履歴は保持し、ロールバック時にも廃止ジャンルの選択・出題は復元しない。
  def down, do: :ok
end
