defmodule WordCounter do
  def count_words(text, target_word, parent_pid) do
    count =
      text
      |> String.downcase()
      |> String.split(~r/[^\w]+/)
      |> Enum.count(fn w -> w == target_word end)

    # 呼び出し元（親プロセス）のPIDに送信する
    send(parent_pid, {:result, target_word, count})
  end
end

# --- ここからメイン処理 ---

texts = [
  "Elixir is a dynamic, functional language designed for building scalable applications.",
  "Erlang VM makes Elixir processes lightweight and concurrent. Elixir is great!",
  "Learning Elixir changes the way you think about programming."
]

target = "elixir"

IO.puts("--- 処理開始 ---")

# メイン処理（親プロセス）のPIDを取得しておく
parent_pid = self()

Enum.each(texts, fn text ->
  spawn(fn -> WordCounter.count_words(text, target, parent_pid) end)
end)

Enum.each(texts, fn _ ->
  receive do
    {:result, ^target, count} ->
      IO.puts("見つかった '#{target}' の数: #{count}")
  after
    1000 ->
      IO.puts("タイムアウトしました")
  end
end)

IO.puts("--- 処理完了 ---")

