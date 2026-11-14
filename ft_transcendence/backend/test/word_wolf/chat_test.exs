defmodule WordWolf.ChatTest do
  use WordWolf.DataCase, async: true

  alias WordWolf.Chat
  alias WordWolf.Games.Game

  # 開発シード（WordWolf.DevSeeds）:
  #   game_history_001 … RESULT。プレイヤー user_yu ほか。チャット msg_1..msg_4（昇順）。
  #   game_history_002 … RESULT。プレイヤー user_yu。チャット無し。
  #   game_case_099    … VOTING。プレイヤー user_hana/user_tomo/user_rio/user_sora、観戦者 user_mei。
  setup do
    WordWolf.DevSeeds.run()
    :ok
  end

  describe "list_messages/1" do
    test "送信時刻の昇順で、契約 ChatMessage の形で返す" do
      messages = Chat.list_messages("game_history_001")

      assert Enum.map(messages, & &1.messageId) == ~w(msg_1 msg_2 msg_3 msg_4)

      assert Enum.map(messages, & &1.content) == [
               "静かに過ごすことが多い場所ですよね。",
               "私は休日にもよく行きます。長居することもあります。",
               "そこで何かを借りることはありますか？",
               "借りることもありますが、読むだけの時もあります。"
             ]

      first = hd(messages)
      assert first.userId == "user_aoi"
      assert first.displayName == "アオイ"
      assert is_binary(first.sentAt)
      assert first |> Map.keys() |> Enum.sort() == [:content, :displayName, :messageId, :sentAt, :userId]
    end

    test "チャットが無いゲームは空リスト" do
      assert Chat.list_messages("game_history_002") == []
    end
  end

  describe "member?/2 と game_exists?/1" do
    test "プレイヤーも観戦者も member?、部外者は false" do
      assert Chat.member?("game_history_001", "user_yu")
      assert Chat.member?("game_case_099", "user_mei")
      refute Chat.member?("game_case_099", "user_yu")
    end

    test "game_exists?/1 は存在で true / 未知で false" do
      assert Chat.game_exists?("game_history_001")
      refute Chat.game_exists?("game_nope")
    end
  end

  describe "publish/3" do
    # DISCUSSION フェーズのゲームがシードに無いので、参加者のそろっている
    # game_case_099 を DISCUSSION に倒して使う。
    setup do
      Repo.update_all(from(g in Game, where: g.id == "game_case_099"),
        set: [phase: "DISCUSSION", paused: false]
      )

      :ok
    end

    test "本文を trim して保存し、契約 ChatMessage の形で返す" do
      assert {:ok, message} = Chat.publish("game_case_099", "user_hana", "  こんばんは  ")

      assert message.content == "こんばんは"
      assert message.userId == "user_hana"
      assert message.displayName == "ハナ"
      assert String.starts_with?(message.messageId, "msg_")
      assert is_binary(message.sentAt)

      # 実際に永続化され、履歴の末尾に載る
      assert Chat.list_messages("game_case_099") |> List.last() |> Map.get(:content) == "こんばんは"
    end

    test "観戦者は発言できない（送信できるのは DISCUSSION 中のプレイヤーだけ）" do
      # user_mei は game_case_099 の観戦者。履歴は読めるが送信は不可。
      assert Chat.member?("game_case_099", "user_mei")
      assert {:error, :not_a_player} = Chat.publish("game_case_099", "user_mei", "なるほど")
    end

    test "空文字・空白のみは :empty" do
      assert {:error, :empty} = Chat.publish("game_case_099", "user_hana", "")
      assert {:error, :empty} = Chat.publish("game_case_099", "user_hana", "   ")
    end

    test "trim 後 300 文字ちょうどは通り、超えると :too_long" do
      assert {:ok, _} = Chat.publish("game_case_099", "user_hana", String.duplicate("あ", 300))
      assert {:error, :too_long} = Chat.publish("game_case_099", "user_hana", String.duplicate("あ", 301))
    end

    test "DISCUSSION 以外のフェーズは :invalid_phase" do
      Repo.update_all(from(g in Game, where: g.id == "game_case_099"), set: [phase: "VOTING"])
      assert {:error, :invalid_phase} = Chat.publish("game_case_099", "user_hana", "まだ話せる？")
    end

    test "一時停止中は :game_paused" do
      Repo.update_all(from(g in Game, where: g.id == "game_case_099"), set: [paused: true])
      assert {:error, :game_paused} = Chat.publish("game_case_099", "user_hana", "とまってる？")
    end

    test "参加していないユーザーは :not_a_player" do
      assert {:error, :not_a_player} = Chat.publish("game_case_099", "user_yu", "入ってないよ")
    end

    test "存在しないゲームは :game_not_found" do
      assert {:error, :game_not_found} = Chat.publish("game_nope", "user_hana", "どこ？")
    end
  end
end
