# 開発用シード。実体は WordWolf.DevSeeds（lib/word_wolf/dev_seeds.ex）。
#
#   mix run priv/repo/seeds.exs         # 手動
#   docker compose 起動時にも自動実行される（compose.yaml の backend.command）
#
# テストからは WordWolf.DevSeeds.run/0 を直接呼ぶ（サンドボックス内でロールバックされる）。

WordWolf.DevSeeds.run()

IO.puts("Seeded: CASE-042 lobby, CASE-099 voting, profile, friends, and results.")
IO.puts("Development bearer token for 田中 ユウ: dev-token-yu")

IO.puts(
  "Development login for every seed user (e.g. player@example.com): #{WordWolf.DevSeeds.dev_password()}"
)
