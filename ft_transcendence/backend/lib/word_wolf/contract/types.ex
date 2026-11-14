# ==========================================================================
# 自動生成ファイル — 手で編集しない
#
#   生成元: _docs/spec/api/openapi.yaml
#   生成:   make gen  （_tools/gen-contract-types.py）
#
# 契約を変えるときは契約PRでYAMLを直し、make gen を実行してコミットする。
# ==========================================================================
# credo:disable-for-this-file

defmodule WordWolf.Contract.Types do
  @moduledoc """
  REST契約（openapi.yaml）のスキーマに対応する型仕様。

  キーはJSONと同じcamelCaseのアトム。日時とIDは `String.t()`。
  列挙は `*_values/0` で文字列一覧を取得できる（Ecto.Enum や検証で使う）。
  """

  @type ok_envelope :: %{
          required(:status) => :ok
        }

  @type error_response :: %{
          required(:status) => :error,
          required(:error) => %{
            required(:code) => error_code(),
            required(:message) => String.t(),
            optional(:details) => map()
          }
        }

  # Channel契約（asyncapi.yaml）のエラーコードと共通の語彙を使う
  @type error_code ::
          :UNAUTHENTICATED
          | :INVALID_CREDENTIALS
          | :FORBIDDEN
          | :NOT_FOUND
          | :INVALID_PAYLOAD
          | :EMAIL_TAKEN
          | :ROOM_FULL
          | :ALREADY_JOINED
          | :GAME_IN_PROGRESS
          | :NOT_A_PLAYER
          | :NOT_HOST
          | :NOT_READY
          | :INVALID_PHASE
          | :INVALID_TARGET
          | :ALREADY_SUBMITTED
          | :DEADLINE_EXCEEDED
          | :RATE_LIMITED
          | :INTERNAL_ERROR

  @spec error_code_values() :: [String.t()]
  def error_code_values do
    ~w(
      UNAUTHENTICATED INVALID_CREDENTIALS FORBIDDEN NOT_FOUND INVALID_PAYLOAD
      EMAIL_TAKEN ROOM_FULL ALREADY_JOINED GAME_IN_PROGRESS NOT_A_PLAYER
      NOT_HOST NOT_READY INVALID_PHASE INVALID_TARGET ALREADY_SUBMITTED
      DEADLINE_EXCEEDED RATE_LIMITED INTERNAL_ERROR
    )
  end

  # 死活監視の応答。依存先の内部状態や構成情報は含めない
  @type health :: %{
          required(:service) => String.t(),
          required(:version) => String.t(),
          required(:time) => String.t()
        }

  @type register_request :: %{
          required(:displayName) => String.t(),
          required(:email) => String.t(),
          required(:password) => String.t(),
          required(:termsAccepted) => boolean()
        }

  @type login_request :: %{
          required(:email) => String.t(),
          required(:password) => String.t()
        }

  @type auth_session :: %{
          required(:token) => String.t(),
          required(:user) => user_private()
        }

  @type user_public :: %{
          required(:userId) => String.t(),
          required(:displayName) => String.t(),
          required(:avatarUrl) => String.t()
        }

  @type user_profile :: %{
          required(:userId) => String.t(),
          required(:displayName) => String.t(),
          required(:avatarUrl) => String.t(),
          required(:bio) => String.t()
        }

  @type user_private :: %{
          required(:userId) => String.t(),
          required(:displayName) => String.t(),
          required(:avatarUrl) => String.t(),
          required(:email) => String.t(),
          optional(:bio) => String.t(),
          optional(:createdAt) => String.t()
        }

  # 勝率はクライアントで導出する（重複した計算値を持たない）
  @type user_stats :: %{
          required(:gamesPlayed) => integer(),
          required(:wins) => integer(),
          required(:losses) => integer()
        }

  @type me_data :: %{
          required(:user) => user_private(),
          required(:stats) => user_stats(),
          required(:friendCount) => integer(),
          required(:activeRoom) => room_preview() | nil
        }

  @type update_profile_request :: %{
          optional(:displayName) => String.t(),
          optional(:bio) => String.t()
        }

  @type avatar_preset_request :: %{
          required(:presetId) => String.t()
        }

  @type match_history_item :: %{
          required(:gameId) => String.t(),
          required(:roomName) => String.t(),
          required(:role) => role(),
          required(:outcome) => :WIN | :LOSS,
          required(:playedAt) => String.t()
        }

  @type pagination_meta :: %{
          required(:total) => integer(),
          required(:page) => integer(),
          required(:limit) => integer()
        }

  # 選択可能なお題ジャンル。恋愛・本音と価値観は廃止し、新規作成・設定変更では指定できない。
  @type room_genre :: :PLACE | :FOOD | :HOBBY | :DAILY_ITEM | :WORK_SCHOOL | :ENTERTAINMENT

  @spec room_genre_values() :: [String.t()]
  def room_genre_values, do: ~w(PLACE FOOD HOBBY DAILY_ITEM WORK_SCHOOL ENTERTAINMENT)

  # お題ジャンル。PLACE=場所, FOOD=食べ物, HOBBY=趣味, DAILY_ITEM=日用品, WORK_SCHOOL=学校・仕事, ENTERTAINMENT=エンタメ, ROMANCE/VAL…
  @type genre ::
          :PLACE
          | :FOOD
          | :HOBBY
          | :DAILY_ITEM
          | :WORK_SCHOOL
          | :ENTERTAINMENT
          | :ROMANCE
          | :VALUES

  @spec genre_values() :: [String.t()]
  def genre_values, do: ~w(PLACE FOOD HOBBY DAILY_ITEM WORK_SCHOOL ENTERTAINMENT ROMANCE VALUES)

  # WAITING=募集中, IN_GAME=ゲーム進行中, CLOSED=終了
  @type room_status :: :WAITING | :IN_GAME | :CLOSED

  @spec room_status_values() :: [String.t()]
  def room_status_values, do: ~w(WAITING IN_GAME CLOSED)

  # COMEBACKは逆転回答の30秒間。ゲーム開始直後の3秒カウントダウン（画面11）はクライアント演出であり、独立フェーズにしない
  @type phase :: :LOBBY | :DEALING | :DISCUSSION | :VOTING | :COMEBACK | :RESULT

  @spec phase_values() :: [String.t()]
  def phase_values, do: ~w(LOBBY DEALING DISCUSSION VOTING COMEBACK RESULT)

  @type participant_type :: :PLAYER | :SPECTATOR

  @spec participant_type_values() :: [String.t()]
  def participant_type_values, do: ~w(PLAYER SPECTATOR)

  # CITIZEN=市民（多数派）, WORD_WOLF=ワードウルフ（少数派）。COMEBACKでは回答者だけウルフと公開し、それ以外は結果確定まで本人にも公開しない
  @type role :: :CITIZEN | :WORD_WOLF

  @spec role_values() :: [String.t()]
  def role_values, do: ~w(CITIZEN WORD_WOLF)

  @type spectator_assignments :: %{
          required(:gameId) => String.t(),
          required(:category) => genre(),
          required(:players) => [%{
            required(:userId) => String.t(),
            required(:displayName) => String.t(),
            required(:avatarUrl) => String.t(),
            required(:word) => String.t(),
            required(:role) => role()
          }]
        }

  @type room_settings_input :: %{
          optional(:discussionMs) => integer(),
          optional(:voteMs) => integer(),
          required(:roomName) => String.t(),
          required(:genres) => [room_genre()],
          optional(:comebackEnabled) => boolean()
        }

  @type room_settings :: %{
          required(:discussionMs) => integer(),
          required(:voteMs) => integer(),
          required(:roomName) => String.t(),
          required(:genres) => [room_genre()],
          required(:comebackEnabled) => boolean(),
          required(:minPlayers) => integer(),
          required(:maxPlayers) => integer(),
          required(:reconnectGraceMs) => integer()
        }

  # 招待確認・ホームカード用の部屋概要。参加者の個人情報は含めない
  @type room_preview :: %{
          required(:roomId) => String.t(),
          required(:inviteCode) => String.t(),
          required(:roomName) => String.t(),
          required(:status) => room_status(),
          required(:genres) => [room_genre()],
          required(:playerCount) => integer(),
          required(:maxPlayers) => integer(),
          required(:spectatorCount) => integer(),
          required(:joinable) => boolean()
        }

  @type room_player :: %{
          required(:userId) => String.t(),
          required(:displayName) => String.t(),
          required(:avatarUrl) => String.t(),
          required(:isHost) => boolean(),
          required(:ready) => boolean(),
          required(:connected) => boolean()
        }

  @type room_state :: %{
          required(:roomId) => String.t(),
          required(:roomName) => String.t(),
          required(:status) => room_status(),
          required(:inviteCode) => String.t(),
          required(:inviteUrl) => String.t(),
          required(:hostUserId) => String.t(),
          required(:settings) => room_settings(),
          required(:players) => [room_player()],
          required(:spectators) => [user_public()],
          required(:currentGameId) => String.t() | nil
        }

  # 参加後にJoinすべきPhoenix ChannelのTopic
  @type realtime_topics :: %{
          required(:roomTopic) => String.t(),
          required(:userTopic) => String.t()
        }

  @type game_player :: %{
          required(:userId) => String.t(),
          required(:displayName) => String.t(),
          required(:avatarUrl) => String.t(),
          required(:connected) => boolean()
        }

  # 誰が見ても同じ公開状態。役職・各自のお題・個別の投票先は含めない
  @type game_public_state :: %{
          required(:comeback) => comeback_state() | nil,
          required(:gameId) => String.t(),
          required(:roomId) => String.t(),
          required(:phase) => phase(),
          required(:phaseVersion) => integer(),
          required(:deadline) => String.t() | nil,
          required(:paused) => boolean(),
          required(:reconnectDeadline) => String.t() | nil,
          required(:startedAt) => String.t(),
          required(:players) => [game_player()],
          required(:spectatorCount) => integer(),
          required(:voteProgress) => vote_progress() | nil
        }

  # 本人だけの秘密情報。多数派・少数派の別（役職）は含めない
  @type assignment :: %{
          required(:gameId) => String.t(),
          required(:word) => String.t(),
          required(:category) => genre()
        }

  # `messageId` と `sentAt` はサーバーが発行する
  @type chat_message :: %{
          required(:messageId) => String.t(),
          required(:userId) => String.t(),
          required(:displayName) => String.t(),
          required(:content) => String.t(),
          required(:sentAt) => String.t()
        }

  @type vote_progress :: %{
          required(:votedCount) => integer(),
          required(:playerCount) => integer()
        }

  @type result_player :: %{
          required(:userId) => String.t(),
          required(:displayName) => String.t(),
          required(:avatarUrl) => String.t(),
          required(:role) => role(),
          required(:word) => String.t(),
          required(:votesReceived) => integer()
        }

  @type game_result :: %{
          required(:comeback) => comeback_result() | nil,
          required(:gameId) => String.t(),
          required(:winner) => :CITIZEN_SIDE | :WOLF_SIDE,
          required(:reason) => :WOLF_IDENTIFIED | :WOLF_ESCAPED | :WOLF_GUESSED,
          required(:wolfUserId) => String.t(),
          required(:words) => %{
            required(:majority) => result_word(),
            required(:minority) => result_word()
          },
          required(:players) => [result_player()],
          required(:myVote) => %{
            required(:targetUserId) => String.t()
          } | nil
        }

  # 逆転回答の公開状態。正解のお題は含めず、入力内容だけを全員へ公開する
  @type comeback_state :: %{
          required(:gameId) => String.t(),
          required(:wolfUserId) => String.t(),
          required(:content) => String.t()
        }

  @type comeback_result :: %{
          required(:answer) => String.t(),
          required(:correct) => boolean(),
          required(:timedOut) => boolean()
        }

  @type result_word :: %{
          required(:word) => String.t(),
          required(:count) => integer()
        }

  @type friend :: %{
          required(:user) => user_public(),
          required(:presence) => friend_presence()
        }

  # 表示初期値のスナップショット。リアルタイム更新はChannelで受け取る
  @type friend_presence :: %{
          required(:state) => :ONLINE | :IN_GAME | :OFFLINE,
          required(:lastSeenAt) => String.t() | nil
        }

  @type friend_request :: %{
          required(:requestId) => String.t(),
          required(:from) => user_public(),
          optional(:to) => user_public(),
          required(:sentAt) => String.t()
        }

  @type room_invite :: %{
          required(:inviteId) => String.t(),
          required(:roomId) => String.t(),
          required(:inviteCode) => String.t(),
          required(:roomName) => String.t(),
          required(:from) => user_public(),
          required(:sentAt) => String.t()
        }

end
