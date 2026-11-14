defmodule WordWolfWeb.ChannelCase do
  @moduledoc """
  Channel / Socket のテスト用ケース。`connect/2`, `subscribe_and_join/3`, `push/3` などを使える。
  DB は Ecto SQL Sandbox（テストごとにロールバック）。
  """

  use ExUnit.CaseTemplate

  using do
    quote do
      import Phoenix.ChannelTest
      import WordWolfWeb.ChannelCase
      import WordWolf.AccountsFixtures
      import WordWolf.GameFixtures

      @endpoint WordWolfWeb.Endpoint
    end
  end

  setup tags do
    WordWolf.DataCase.setup_sandbox(tags)
    :ok
  end
end
