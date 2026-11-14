# 開発用。Elixir 1.19.5 / OTP 28 に固定（_docs/dev/tools.html の方針どおり期間中は上げない）。
FROM elixir:1.19-alpine

RUN apk add --no-cache build-base git inotify-tools
WORKDIR /app
RUN mix local.hex --force && mix local.rebar --force

COPY mix.exs mix.lock ./
RUN mix deps.get

COPY . .

EXPOSE 4000
CMD ["sh", "-c", "mix ecto.create --quiet && mix ecto.migrate --quiet && mix phx.server"]
