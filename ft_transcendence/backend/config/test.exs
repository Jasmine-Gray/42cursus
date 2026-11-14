import Config

# Configure your database
#
# The MIX_TEST_PARTITION environment variable can be used
# to provide built-in test partitioning in CI environment.
# Run `mix help test` for more information.
config :word_wolf, WordWolf.Repo,
  username: System.get_env("POSTGRES_USER", "postgres"),
  password: System.get_env("POSTGRES_PASSWORD", "postgres"),
  hostname: System.get_env("PGHOST", "localhost"),
  database: "word_wolf_test#{System.get_env("MIX_TEST_PARTITION")}",
  pool: Ecto.Adapters.SQL.Sandbox,
  pool_size: System.schedulers_online() * 2

# We don't run a server during test. If one is required,
# you can enable the server option below.
config :word_wolf, WordWolfWeb.Endpoint,
  http: [ip: {127, 0, 0, 1}, port: 4002],
  secret_key_base: "q1qPe1Sg+SbnAg7Yf/2tpZ8F9N0UJgJXtGuoBqo80QKWYq4smfOW8rvKNbDImXbd",
  server: false

# GameServer の固定長フェーズ（既定 DEALING 3秒 / RESULT 30秒）を詰める。
# DISCUSSION を要するテストが実時間を待たずに済む（lib/word_wolf/games/game_server.ex）。
config :word_wolf, dealing_ms: 10, result_ms: 50

# Print only warnings and errors during test
config :logger, level: :warning

# Initialize plugs at runtime for faster test compilation
config :phoenix, :plug_init_mode, :runtime

# Sort query params output of verified routes for robust url comparisons
config :phoenix,
  sort_verified_routes_query_params: true

# テストでは bcrypt のコストを最小にする（登録・ログイン・DevSeeds が毎テスト走る）
config :bcrypt_elixir, log_rounds: 1
