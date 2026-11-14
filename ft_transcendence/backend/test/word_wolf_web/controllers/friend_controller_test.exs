defmodule WordWolfWeb.FriendControllerTest do
  use WordWolfWeb.ConnCase, async: true

  alias WordWolf.Friends

  defp setup_users(_context) do
    user_a = user_fixture(%{display_name: "アリス", email: "alice@example.com", avatar_url: "/avatars/yu.webp"})
    user_b = user_fixture(%{display_name: "ボブ", email: "bob@example.com", avatar_url: "/avatars/minato.webp"})
    user_c = user_fixture(%{display_name: "チャーリー", email: "charlie@example.com", avatar_url: "/avatars/cat.webp"})

    {_sess_a, token_a} = session_fixture(user_a)
    {_sess_b, token_b} = session_fixture(user_b)
    {_sess_c, token_c} = session_fixture(user_c)

    {:ok,
     user_a: user_a,
     token_a: token_a,
     user_b: user_b,
     token_b: token_b,
     user_c: user_c,
     token_c: token_c}
  end

  describe "GET /users (searchUsers)" do
    setup :setup_users

    test "表示名でユーザーを部分一致検索できる（自分自身は含まれない）", %{
      conn: conn,
      token_a: token_a,
      user_b: user_b
    } do
      conn =
        conn
        |> authenticate_user(token_a)
        |> get(~p"/users", %{"q" => "ボブ"})

      assert %{"status" => "ok", "data" => %{"items" => items}} = json_response(conn, 200)
      assert length(items) == 1
      assert hd(items)["userId"] == user_b.id
      assert hd(items)["displayName"] == "ボブ"
      assert hd(items)["avatarUrl"] == user_b.avatar_url
    end

    test "検索クエリが2文字未満の場合は 400 INVALID_PAYLOAD", %{conn: conn, token_a: token_a} do
      conn =
        conn
        |> authenticate_user(token_a)
        |> get(~p"/users", %{"q" => "a"})

      assert %{"status" => "error", "error" => %{"code" => "INVALID_PAYLOAD"}} =
               json_response(conn, 400)
    end

    test "未認証リクエストは 401 UNAUTHENTICATED", %{conn: conn} do
      conn = get(conn, ~p"/users", %{"q" => "ボブ"})
      assert %{"status" => "error", "error" => %{"code" => "UNAUTHENTICATED"}} = json_response(conn, 401)
    end
  end

  describe "POST /friends/requests (sendFriendRequest)" do
    setup :setup_users

    test "相手へフレンド申請を送信できる", %{
      conn: conn,
      token_a: token_a,
      user_b: user_b
    } do
      conn =
        conn
        |> authenticate_user(token_a)
        |> post(~p"/friends/requests", %{"targetUserId" => user_b.id})

      assert %{"status" => "ok", "data" => %{"requestId" => request_id}} =
               json_response(conn, 201)

      assert is_binary(request_id)
      assert String.starts_with?(request_id, "freq_")
    end

    test "自分自身への申請は 400 INVALID_PAYLOAD", %{conn: conn, token_a: token_a, user_a: user_a} do
      conn =
        conn
        |> authenticate_user(token_a)
        |> post(~p"/friends/requests", %{"targetUserId" => user_a.id})

      assert %{"status" => "error", "error" => %{"code" => "INVALID_PAYLOAD"}} =
               json_response(conn, 400)
    end

    test "存在しないユーザーへの申請は 404 NOT_FOUND", %{conn: conn, token_a: token_a} do
      conn =
        conn
        |> authenticate_user(token_a)
        |> post(~p"/friends/requests", %{"targetUserId" => "user_nonexistent"})

      assert %{"status" => "error", "error" => %{"code" => "NOT_FOUND"}} =
               json_response(conn, 404)
    end

    test "既に申請中の場合は 409 ALREADY_SUBMITTED", %{
      conn: conn,
      token_a: token_a,
      user_a: user_a,
      user_b: user_b
    } do
      {:ok, _} = Friends.send_friend_request(user_a.id, user_b.id)

      conn =
        conn
        |> authenticate_user(token_a)
        |> post(~p"/friends/requests", %{"targetUserId" => user_b.id})

      assert %{"status" => "error", "error" => %{"code" => "ALREADY_SUBMITTED"}} =
               json_response(conn, 409)
    end
  end

  describe "GET /friends/requests, accept, decline, GET /friends" do
    setup :setup_users

    test "受信したフレンド申請の取得・承認・フレンド一覧への反映", %{
      conn: conn,
      token_a: token_a,
      token_b: token_b,
      user_a: user_a,
      user_b: user_b
    } do
      # 1. アリスからボブへ申請送信
      {:ok, %{requestId: req_id}} = Friends.send_friend_request(user_a.id, user_b.id)

      # 2. ボブが受信申請一覧を取得
      conn_b =
        conn
        |> authenticate_user(token_b)
        |> get(~p"/friends/requests")

      assert %{"status" => "ok", "data" => %{"items" => requests}} = json_response(conn_b, 200)
      assert length(requests) == 1
      assert hd(requests)["requestId"] == req_id
      assert hd(requests)["from"]["userId"] == user_a.id
      assert hd(requests)["from"]["displayName"] == "アリス"

      # 3. ボブが申請を承認
      conn_b =
        conn
        |> authenticate_user(token_b)
        |> post(~p"/friends/requests/#{req_id}/accept")

      assert %{"status" => "ok", "data" => friend_data} = json_response(conn_b, 200)
      assert friend_data["user"]["userId"] == user_a.id
      assert friend_data["presence"]["state"] != nil

      # 4. ボブのフレンド一覧を取得
      conn_b =
        conn
        |> authenticate_user(token_b)
        |> get(~p"/friends")

      assert %{"status" => "ok", "data" => %{"items" => friends_b}} = json_response(conn_b, 200)
      assert length(friends_b) == 1
      assert hd(friends_b)["user"]["userId"] == user_a.id

      # 5. アリスのフレンド一覧にもボブが反映されている
      conn_a =
        conn
        |> authenticate_user(token_a)
        |> get(~p"/friends")

      assert %{"status" => "ok", "data" => %{"items" => friends_a}} = json_response(conn_a, 200)
      assert length(friends_a) == 1
      assert hd(friends_a)["user"]["userId"] == user_b.id

      # 6. 承認済みのため受信申請一覧は空になる
      conn_b =
        conn
        |> authenticate_user(token_b)
        |> get(~p"/friends/requests")

      assert %{"status" => "ok", "data" => %{"items" => []}} = json_response(conn_b, 200)
    end

    test "フレンド申請の拒否と送信側（direction=outgoing）での自動消去", %{
      conn: conn,
      token_a: token_a,
      token_b: token_b,
      user_a: user_a,
      user_b: user_b
    } do
      {:ok, %{requestId: req_id}} = Friends.send_friend_request(user_a.id, user_b.id)

      # アリスが送信中申請（outgoing）を確認
      conn_a =
        conn
        |> authenticate_user(token_a)
        |> get(~p"/friends/requests", %{"direction" => "outgoing"})

      assert %{"status" => "ok", "data" => %{"items" => sent_requests}} =
               json_response(conn_a, 200)

      assert length(sent_requests) == 1
      assert hd(sent_requests)["requestId"] == req_id
      assert hd(sent_requests)["to"]["userId"] == user_b.id

      # ボブが申請を拒否
      conn_b =
        conn
        |> authenticate_user(token_b)
        |> post(~p"/friends/requests/#{req_id}/decline")

      assert %{"status" => "ok"} = json_response(conn_b, 200)

      # 拒否後はボブの受信申請一覧は空
      conn_b =
        conn
        |> authenticate_user(token_b)
        |> get(~p"/friends/requests")

      assert %{"status" => "ok", "data" => %{"items" => []}} = json_response(conn_b, 200)

      # 拒否後はアリスの送信中申請一覧（outgoing）からも消えている
      conn_a =
        conn
        |> authenticate_user(token_a)
        |> get(~p"/friends/requests", %{"direction" => "outgoing"})

      assert %{"status" => "ok", "data" => %{"items" => []}} = json_response(conn_a, 200)
    end
  end

  describe "DELETE /friends/:userId (removeFriend)" do
    setup :setup_users

    defp befriend(requester, addressee) do
      {:ok, %{requestId: request_id}} = Friends.send_friend_request(requester.id, addressee.id)
      {:ok, _} = Friends.accept_friend_request(addressee.id, request_id)
    end

    test "フレンドを解除すると双方の一覧から消える", %{
      conn: conn,
      token_a: token_a,
      user_a: user_a,
      user_b: user_b,
      user_c: user_c
    } do
      befriend(user_a, user_b)
      befriend(user_c, user_a)

      conn =
        conn
        |> authenticate_user(token_a)
        |> delete(~p"/friends/#{user_b.id}")

      assert %{"status" => "ok"} = json_response(conn, 200)
      assert [%{user: %{userId: remaining}}] = Friends.list_friends(user_a.id)
      assert remaining == user_c.id
      assert Friends.list_friends(user_b.id) == []
    end

    test "相手から申請されたフレンドも解除できる", %{
      conn: conn,
      token_a: token_a,
      user_a: user_a,
      user_b: user_b
    } do
      befriend(user_b, user_a)

      conn =
        conn
        |> authenticate_user(token_a)
        |> delete(~p"/friends/#{user_b.id}")

      assert %{"status" => "ok"} = json_response(conn, 200)
      assert Friends.list_friends(user_a.id) == []
    end

    test "解除後は改めてフレンド申請できる", %{
      conn: conn,
      token_a: token_a,
      user_a: user_a,
      user_b: user_b
    } do
      befriend(user_a, user_b)

      conn
      |> authenticate_user(token_a)
      |> delete(~p"/friends/#{user_b.id}")
      |> json_response(200)

      assert {:ok, %{requestId: _}} = Friends.send_friend_request(user_a.id, user_b.id)
    end

    test "フレンドでない相手（申請中を含む）は 404 NOT_FOUND で、申請は残る", %{
      conn: conn,
      token_a: token_a,
      user_a: user_a,
      user_b: user_b
    } do
      {:ok, _} = Friends.send_friend_request(user_a.id, user_b.id)

      conn =
        conn
        |> authenticate_user(token_a)
        |> delete(~p"/friends/#{user_b.id}")

      assert %{"status" => "error", "error" => %{"code" => "NOT_FOUND"}} =
               json_response(conn, 404)

      assert [_pending] = Friends.list_friend_requests(user_b.id, "incoming")
    end

    test "存在しないユーザーは 404 NOT_FOUND", %{conn: conn, token_a: token_a} do
      conn =
        conn
        |> authenticate_user(token_a)
        |> delete(~p"/friends/user_nonexistent")

      assert %{"status" => "error", "error" => %{"code" => "NOT_FOUND"}} =
               json_response(conn, 404)
    end

    test "未認証リクエストは 401 UNAUTHENTICATED", %{conn: conn, user_b: user_b} do
      conn = delete(conn, ~p"/friends/#{user_b.id}")

      assert %{"status" => "error", "error" => %{"code" => "UNAUTHENTICATED"}} =
               json_response(conn, 401)
    end
  end
end
