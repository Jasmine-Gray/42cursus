defmodule WordWolfWeb.UserSocketTest do
  use WordWolfWeb.ChannelCase, async: true

  alias WordWolfWeb.UserSocket

  test "有効なセッショントークンで接続でき、assigns に user_id / session_id が入る" do
    user = user_fixture()
    {session, token} = session_fixture(user)

    assert {:ok, socket} = connect(UserSocket, %{"token" => token})
    assert socket.assigns.user_id == user.id
    assert socket.assigns.session_id == session.id
    assert UserSocket.id(socket) == "user_socket:#{user.id}"
  end

  test "別のトークンなら別のユーザーとして接続される（複数タブ = 複数プレイヤー）" do
    {_s1, token1} = session_fixture(user_fixture())
    {_s2, token2} = session_fixture(user_fixture())

    assert {:ok, socket1} = connect(UserSocket, %{"token" => token1})
    assert {:ok, socket2} = connect(UserSocket, %{"token" => token2})
    refute socket1.assigns.user_id == socket2.assigns.user_id
  end

  test "無効なトークン・トークンなしは接続を拒否する" do
    assert :error = connect(UserSocket, %{"token" => "not-a-token"})
    assert :error = connect(UserSocket, %{"token" => ""})
    assert :error = connect(UserSocket, %{})
  end

  test "開発シードの dev-token-<name> でも接続できる（/dev/as/<name> のタブがそのまま WebSocket に乗る）" do
    WordWolf.DevSeeds.run()

    assert {:ok, socket} = connect(UserSocket, %{"token" => "dev-token-minato"})
    assert socket.assigns.user_id == "user_minato"
  end
end
