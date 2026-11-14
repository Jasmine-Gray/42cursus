defmodule WordWolf.AccountsFixtures do
  @moduledoc """
  テスト用のユーザーおよびセッション作成ヘルパー。
  """

  alias WordWolf.Accounts.{Session, User}
  alias WordWolf.Repo

  def user_fixture(attrs \\ %{}) do
    unique_id = System.unique_integer([:positive])
    now = DateTime.utc_now() |> DateTime.truncate(:microsecond)

    params =
      Enum.into(attrs, %{
        id: "user_test_#{unique_id}",
        email: "user_#{unique_id}@example.com",
        password_hash: "hashed_password",
        display_name: "テストユーザー #{unique_id}",
        avatar_url: "/avatars/yu.webp",
        bio: "テスト用自己紹介です",
        presence_state: "ONLINE",
        terms_accepted_at: now
      })

    %User{}
    |> Ecto.Changeset.change(params)
    |> Repo.insert!()
  end

  def session_fixture(user, token \\ nil) do
    unique_id = System.unique_integer([:positive])
    token = token || "test-token-#{unique_id}"
    token_hash = WordWolf.Accounts.hash_token(token)
    now = DateTime.utc_now() |> DateTime.truncate(:microsecond)
    expires_at = DateTime.add(now, 3600, :second)

    session =
      %Session{}
      |> Ecto.Changeset.change(%{
        id: "session_test_#{unique_id}",
        user_id: user.id,
        token_hash: token_hash,
        expires_at: expires_at,
        last_used_at: now
      })
      |> Repo.insert!()

    {session, token}
  end
end
