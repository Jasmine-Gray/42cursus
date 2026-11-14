defmodule WordWolf.DevSeeds do
  @moduledoc """
  開発・テスト用のシードデータ（画面確認用シナリオ）。

  投入口は `priv/repo/seeds.exs`（`mix run priv/repo/seeds.exs` / docker compose 起動時）。
  テストからは `WordWolf.DevSeeds.run/0` を直接呼ぶ（サンドボックス内でロールバックされる）。

  データ: CASE-042 ロビー / CASE-099 投票中 / プロフィール / フレンド / 結果履歴。
  ログイン: 全ユーザー共通のパスワードは `dev_password/0`。田中 ユウ の Bearer は `dev-token-yu`。
  """

  alias WordWolf.Accounts.{Session, User}
  alias WordWolf.Friends.Friendship
  alias WordWolf.Games.{ChatMessage, Game, GamePlayer, Vote}
  alias WordWolf.Repo
  alias WordWolf.Rooms.{Room, RoomMember, WordGroup}
  import Ecto.Query

  # 全シードユーザー共通の開発用パスワード（画面01 ログインの動作確認用）。
  # bcrypt は遅いのでハッシュは run/0 の中で1回だけ計算し、全ユーザーで共有する。
  @dev_password "password123"
  @mix_env Mix.env()

  @doc "シードユーザー全員に共通の開発用パスワード（メールは users/1 を参照）。"
  @spec dev_password() :: String.t()
  def dev_password, do: @dev_password

  def run do
    if @mix_env == :prod and System.get_env("ALLOW_DEV_SEEDS") != "true" do
      raise "development seed data is disabled in production"
    end

    password_hash = Bcrypt.hash_pwd_salt(@dev_password)

    Repo.transaction(fn ->
      upsert(User, users(password_hash), [
        :email,
        :password_hash,
        :display_name,
        :avatar_url,
        :bio,
        :presence_state,
        :last_seen_at,
        :terms_accepted_at,
        :updated_at
      ])

      upsert(Session, sessions(password_hash), [:user_id, :token_hash, :expires_at, :last_used_at])

      upsert(Friendship, friendships(), [
        :requester_id,
        :addressee_id,
        :status,
        :responded_at,
        :updated_at
      ])

      upsert(Room, rooms(), [
        :host_user_id,
        :room_name,
        :invite_code,
        :status,
        :genres,
        :discussion_ms,
        :vote_ms,
        :min_players,
        :max_players,
        :reconnect_grace_ms,
        :closed_at,
        :updated_at
      ])

      upsert(RoomMember, room_members(), [
        :room_id,
        :user_id,
        :participant_type,
        :ready,
        :connected,
        :joined_at,
        :updated_at
      ])

      sync_word_groups()

      upsert(Game, games(), [
        :room_id,
        :word_group_id,
        :phase,
        :phase_version,
        :deadline,
        :paused,
        :reconnect_deadline,
        :winner,
        :result_reason,
        :started_at,
        :ended_at,
        :aborted_at,
        :updated_at
      ])

      upsert(GamePlayer, game_players(), [
        :game_id,
        :user_id,
        :participant_type,
        :role,
        :assigned_word,
        :connected,
        :updated_at
      ])

      upsert(ChatMessage, chat_messages(), [:game_id, :user_id, :content, :sent_at])
      upsert(Vote, votes(), [:game_id, :voter_user_id, :target_user_id, :voted_at])
    end)

    :ok
  end

  defp upsert(schema, rows, replace_fields) do
    Repo.insert_all(
      schema,
      rows,
      conflict_target: [:id],
      on_conflict: {:replace, replace_fields}
    )
  end

  defp users(password_hash) do
    [
      user("user_yu", "player@example.com", "田中 ユウ", "yu", "推理ゲームが好きです", "ONLINE", password_hash),
      user(
        "user_minato",
        "minato@example.com",
        "ミナト",
        "minato",
        "休日は読書をしています",
        "ONLINE",
        password_hash
      ),
      user("user_aoi", "aoi@example.com", "アオイ", "aoi", "ワードウルフ初心者です", "ONLINE", password_hash),
      user("user_ren", "ren@example.com", "レン", "ren", "静かな場所が好きです", "OFFLINE", password_hash),
      user("user_yuna", "yuna@example.com", "ユナ", "yuna", "みんなで遊びましょう", "OFFLINE", password_hash),
      user("user_kei", "kei@example.com", "ケイ", "kei", "推理は慎重派です", "ONLINE", password_hash),
      user("user_nao", "nao@example.com", "ナオ", "owl", "今日は観戦します", "ONLINE", password_hash),
      user(
        "user_hana",
        "hana@example.com",
        "ハナ",
        "child-detective",
        nil,
        "IN_GAME",
        password_hash
      ),
      user("user_tomo", "tomo@example.com", "トモ", "cat", nil, "IN_GAME", password_hash),
      user("user_rio", "rio@example.com", "リオ", "shiba", nil, "IN_GAME", password_hash),
      user("user_sora", "sora@example.com", "ソラ", "rabbit", nil, "IN_GAME", password_hash),
      user("user_mei", "mei@example.com", "メイ", "senior-detective", nil, "ONLINE", password_hash)
    ]
  end

  defp user(id, email, display_name, avatar, bio, presence_state, password_hash) do
    timestamp = utc(~N[2026-08-01 00:00:00])

    %{
      id: id,
      email: email,
      password_hash: password_hash,
      display_name: display_name,
      avatar_url: "/avatars/#{avatar}.webp",
      bio: bio,
      presence_state: presence_state,
      last_seen_at: if(presence_state == "OFFLINE", do: utc(~N[2026-08-21 12:00:00])),
      terms_accepted_at: timestamp,
      inserted_at: timestamp,
      updated_at: timestamp
    }
  end

  # 全シードユーザーに Bearer "dev-token-<name>"（<name> は id の user_ を除いた部分）のセッションを作る。
  # 開発用ページ /dev/as/<name>（frontend/src/pages/dev-login-page.tsx）がこのトークンでタブ単位にログインする。
  defp sessions(password_hash) do
    for %{id: "user_" <> suffix = id} <- users(password_hash) do
      %{
        id: "session_#{suffix}",
        user_id: id,
        token_hash: token_hash("dev-token-#{suffix}"),
        expires_at: utc(~N[2030-01-01 00:00:00]),
        last_used_at: utc(~N[2026-08-22 00:00:00]),
        inserted_at: utc(~N[2026-08-01 00:00:00])
      }
    end
  end

  defp friendships do
    accepted_at = utc(~N[2026-08-10 00:00:00])

    [
      friendship("friend_yu_minato", "user_yu", "user_minato", "ACCEPTED", accepted_at),
      friendship("friend_aoi_yu", "user_aoi", "user_yu", "ACCEPTED", accepted_at),
      friendship("friend_yu_yuna", "user_yu", "user_yuna", "ACCEPTED", accepted_at),
      friendship("freq_ren_yu", "user_ren", "user_yu", "PENDING", nil)
    ]
  end

  defp friendship(id, requester_id, addressee_id, status, responded_at) do
    timestamp = utc(~N[2026-08-09 00:00:00])

    %{
      id: id,
      requester_id: requester_id,
      addressee_id: addressee_id,
      status: status,
      responded_at: responded_at,
      inserted_at: timestamp,
      updated_at: timestamp
    }
  end

  defp rooms do
    [
      room("room_case_042", "user_yu", "深夜の読書会", "CASE-042", "WAITING", ["PLACE", "HOBBY"]),
      room("room_case_099", "user_hana", "夜食の捜査会議", "CASE-099", "IN_GAME", ["FOOD"]),
      room("room_history_001", "user_yu", "図書館の事件", "HISTORY-001", "CLOSED", ["PLACE"]),
      room("room_history_002", "user_yu", "寿司好きの集い", "HISTORY-002", "CLOSED", ["FOOD"]),
      room("room_history_003", "user_yu", "映画研究会", "HISTORY-003", "CLOSED", ["ENTERTAINMENT"])
    ]
  end

  defp room(id, host_user_id, room_name, invite_code, status, genres) do
    timestamp = utc(~N[2026-08-20 11:30:00])

    closed_at =
      %{
        "room_history_001" => utc(~N[2026-08-20 12:06:00]),
        "room_history_002" => utc(~N[2026-08-18 12:06:00]),
        "room_history_003" => utc(~N[2026-08-16 12:06:00])
      }
      |> Map.get(id)

    %{
      id: id,
      host_user_id: host_user_id,
      room_name: room_name,
      invite_code: invite_code,
      status: status,
      genres: genres,
      discussion_ms: 300_000,
      vote_ms: 30_000,
      min_players: 3,
      max_players: 8,
      reconnect_grace_ms: 20_000,
      closed_at: closed_at,
      inserted_at: timestamp,
      updated_at: timestamp
    }
  end

  defp room_members do
    lobby_players = ~w(user_yu user_minato user_aoi user_ren user_yuna user_kei)
    game_players = ~w(user_hana user_tomo user_rio user_sora)

    lobby_rows =
      Enum.with_index(lobby_players, 1)
      |> Enum.map(fn {user_id, index} ->
        room_member("member_042_#{index}", "room_case_042", user_id, "PLAYER", true, index)
      end)

    game_rows =
      Enum.with_index(game_players, 1)
      |> Enum.map(fn {user_id, index} ->
        room_member("member_099_#{index}", "room_case_099", user_id, "PLAYER", true, index)
      end)

    lobby_rows ++
      game_rows ++
      [
        room_member("member_042_spectator", "room_case_042", "user_nao", "SPECTATOR", false, 7),
        room_member("member_099_spectator", "room_case_099", "user_mei", "SPECTATOR", false, 5)
      ]
  end

  defp room_member(id, room_id, user_id, participant_type, ready, order) do
    timestamp = utc(~N[2026-08-20 11:30:00]) |> DateTime.add(order, :second)

    %{
      id: id,
      room_id: room_id,
      user_id: user_id,
      participant_type: participant_type,
      ready: ready,
      connected: true,
      joined_at: timestamp,
      inserted_at: timestamp,
      updated_at: timestamp
    }
  end

  @doc "現行のお題だけを出題対象にする。削除済みのお題は履歴用にDBへ残す。"
  @spec sync_word_groups() :: {:ok, term()} | {:error, term()}
  def sync_word_groups do
    groups = Enum.uniq_by(catalog_groups() ++ word_groups(), & &1.id)
    active_ids = for %{id: id, active: true} <- groups, do: id

    Repo.transaction(fn ->
      from(group in WordGroup, where: group.active and group.id not in ^active_ids)
      |> Repo.update_all(set: [active: false, updated_at: DateTime.utc_now()])

      upsert(WordGroup, groups, [:genre, :words, :active, :updated_at])
    end)
  end

  # 既存シナリオの参照先。出題には使わず、過去の結果を再現するために保持する。
  defp word_groups do
    timestamp = utc(~N[2026-08-01 00:00:00])

    [
      {"word_place_library", "PLACE", "図書館", "書店"},
      {"word_food_sushi", "FOOD", "寿司", "刺身"},
      {"word_entertainment_movie", "ENTERTAINMENT", "映画", "ドラマ"},
      {"word_food_ramen", "FOOD", "ラーメン", "うどん"},
      {"word_hobby_camp", "HOBBY", "キャンプ", "ピクニック"},
      {"word_daily_umbrella", "DAILY_ITEM", "傘", "レインコート"}
    ]
    |> Enum.map(fn {id, genre, majority, minority} ->
      %{
        id: id,
        genre: genre,
        words: [majority, minority],
        active: false,
        inserted_at: timestamp,
        updated_at: timestamp
      }
    end)
  end

  defp catalog_groups do
    path = Application.app_dir(:word_wolf, "priv/word-groups.json")
    timestamp = utc(~N[2026-09-22 00:00:00])

    path
    |> File.read!()
    |> Jason.decode!()
    |> Enum.map(fn group ->
      %{
        id: group["id"],
        genre: group["genre"],
        words: group["words"],
        # 遊んだお題は JSON に "active": false を付けて残す（省略時は出題する）
        active: Map.get(group, "active", true),
        inserted_at: timestamp,
        updated_at: timestamp
      }
    end)
  end

  defp games do
    [
      game("game_case_099", "room_case_099", "word_food_ramen", "VOTING", 3, nil, nil),
      game(
        "game_history_001",
        "room_history_001",
        "word_place_library",
        "RESULT",
        4,
        "CITIZEN_SIDE",
        "WOLF_IDENTIFIED"
      ),
      game(
        "game_history_002",
        "room_history_002",
        "word_food_sushi",
        "RESULT",
        4,
        "WOLF_SIDE",
        "WOLF_ESCAPED"
      ),
      game(
        "game_history_003",
        "room_history_003",
        "word_entertainment_movie",
        "RESULT",
        4,
        "WOLF_SIDE",
        "WOLF_ESCAPED"
      )
    ]
  end

  defp game(id, room_id, word_group_id, phase, phase_version, winner, result_reason) do
    start_times = %{
      "game_case_099" => ~N[2026-08-22 00:00:00],
      "game_history_001" => ~N[2026-08-20 12:00:00],
      "game_history_002" => ~N[2026-08-18 12:00:00],
      "game_history_003" => ~N[2026-08-16 12:00:00]
    }

    started_at = utc(Map.fetch!(start_times, id))
    completed? = phase == "RESULT"

    %{
      id: id,
      room_id: room_id,
      word_group_id: word_group_id,
      phase: phase,
      phase_version: phase_version,
      deadline: if(completed?, do: nil, else: utc(~N[2030-01-01 00:00:00])),
      paused: false,
      reconnect_deadline: nil,
      winner: winner,
      result_reason: result_reason,
      started_at: started_at,
      ended_at: if(completed?, do: DateTime.add(started_at, 360, :second)),
      aborted_at: nil,
      inserted_at: started_at,
      updated_at: started_at
    }
  end

  defp game_players do
    active = [
      player("game_case_099", "user_hana", "CITIZEN", "ラーメン"),
      player("game_case_099", "user_tomo", "WORD_WOLF", "うどん"),
      player("game_case_099", "user_rio", "CITIZEN", "ラーメン"),
      player("game_case_099", "user_sora", "CITIZEN", "ラーメン"),
      spectator("game_case_099", "user_mei")
    ]

    result =
      ~w(user_yu user_minato user_aoi user_ren user_yuna user_kei)
      |> Enum.map(fn user_id ->
        role = if user_id == "user_minato", do: "WORD_WOLF", else: "CITIZEN"
        word = if role == "WORD_WOLF", do: "書店", else: "図書館"
        player("game_history_001", user_id, role, word)
      end)

    history_two = [
      player("game_history_002", "user_yu", "WORD_WOLF", "刺身"),
      player("game_history_002", "user_minato", "CITIZEN", "寿司"),
      player("game_history_002", "user_aoi", "CITIZEN", "寿司"),
      player("game_history_002", "user_ren", "CITIZEN", "寿司")
    ]

    history_three = [
      player("game_history_003", "user_yu", "CITIZEN", "映画"),
      player("game_history_003", "user_minato", "WORD_WOLF", "ドラマ"),
      player("game_history_003", "user_aoi", "CITIZEN", "映画"),
      player("game_history_003", "user_ren", "CITIZEN", "映画")
    ]

    active ++ result ++ history_two ++ history_three
  end

  defp player(game_id, user_id, role, word) do
    game_player(game_id, user_id, "PLAYER", role, word)
  end

  defp spectator(game_id, user_id) do
    game_player(game_id, user_id, "SPECTATOR", nil, nil)
  end

  defp game_player(game_id, user_id, participant_type, role, word) do
    timestamp =
      %{
        "game_case_099" => utc(~N[2026-08-22 00:00:00]),
        "game_history_001" => utc(~N[2026-08-20 12:00:00]),
        "game_history_002" => utc(~N[2026-08-18 12:00:00]),
        "game_history_003" => utc(~N[2026-08-16 12:00:00])
      }
      |> Map.fetch!(game_id)

    %{
      id: "gplayer_#{game_id}_#{user_id}",
      game_id: game_id,
      user_id: user_id,
      participant_type: participant_type,
      role: role,
      assigned_word: word,
      connected: true,
      inserted_at: timestamp,
      updated_at: timestamp
    }
  end

  defp chat_messages do
    [
      message(
        "msg_1",
        "game_history_001",
        "user_aoi",
        "静かに過ごすことが多い場所ですよね。",
        ~N[2026-08-20 12:04:00]
      ),
      message(
        "msg_2",
        "game_history_001",
        "user_minato",
        "私は休日にもよく行きます。長居することもあります。",
        ~N[2026-08-20 12:04:20]
      ),
      message(
        "msg_3",
        "game_history_001",
        "user_yu",
        "そこで何かを借りることはありますか？",
        ~N[2026-08-20 12:05:00]
      ),
      message(
        "msg_4",
        "game_history_001",
        "user_ren",
        "借りることもありますが、読むだけの時もあります。",
        ~N[2026-08-20 12:05:20]
      ),
      message(
        "msg_099_1",
        "game_case_099",
        "user_hana",
        "温かい状態で食べたいですよね。",
        ~N[2026-08-22 00:03:00]
      ),
      message("msg_099_2", "game_case_099", "user_tomo", "私はスープも楽しみです。", ~N[2026-08-22 00:03:20]),
      message(
        "msg_099_3",
        "game_case_099",
        "user_rio",
        "夜に食べたくなることがあります。",
        ~N[2026-08-22 00:04:00]
      )
    ]
  end

  defp message(id, game_id, user_id, content, sent_at) do
    timestamp = utc(sent_at)

    %{
      id: id,
      game_id: game_id,
      user_id: user_id,
      content: content,
      sent_at: timestamp,
      inserted_at: timestamp
    }
  end

  defp votes do
    active = [vote("game_case_099", "user_hana", "user_tomo", 1)]

    result = [
      vote("game_history_001", "user_yu", "user_minato", 1),
      vote("game_history_001", "user_minato", "user_aoi", 2),
      vote("game_history_001", "user_aoi", "user_minato", 3),
      vote("game_history_001", "user_ren", "user_minato", 4),
      vote("game_history_001", "user_yuna", "user_minato", 5),
      vote("game_history_001", "user_kei", "user_ren", 6)
    ]

    history_two = [
      vote("game_history_002", "user_yu", "user_aoi", 1),
      vote("game_history_002", "user_minato", "user_aoi", 2),
      vote("game_history_002", "user_aoi", "user_yu", 3),
      vote("game_history_002", "user_ren", "user_yu", 4)
    ]

    history_three = [
      vote("game_history_003", "user_yu", "user_minato", 1),
      vote("game_history_003", "user_minato", "user_aoi", 2),
      vote("game_history_003", "user_aoi", "user_yu", 3),
      vote("game_history_003", "user_ren", "user_aoi", 4)
    ]

    active ++ result ++ history_two ++ history_three
  end

  defp vote(game_id, voter_user_id, target_user_id, order) do
    timestamp =
      %{
        "game_case_099" => utc(~N[2026-08-22 00:05:30]),
        "game_history_001" => utc(~N[2026-08-20 12:05:30]),
        "game_history_002" => utc(~N[2026-08-18 12:05:30]),
        "game_history_003" => utc(~N[2026-08-16 12:05:30])
      }
      |> Map.fetch!(game_id)
      |> DateTime.add(order, :second)

    %{
      id: "vote_#{game_id}_#{voter_user_id}",
      game_id: game_id,
      voter_user_id: voter_user_id,
      target_user_id: target_user_id,
      voted_at: timestamp,
      inserted_at: timestamp
    }
  end

  defp token_hash(token) do
    :crypto.hash(:sha256, token) |> Base.encode64()
  end

  defp utc(naive_datetime) do
    naive_datetime
    |> DateTime.from_naive!("Etc/UTC")
    |> Map.put(:microsecond, {0, 6})
  end
end
