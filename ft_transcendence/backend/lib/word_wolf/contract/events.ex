# ==========================================================================
# 自動生成ファイル — 手で編集しない
#
#   生成元: _docs/spec/api/asyncapi.yaml
#   生成:   make gen  （_tools/gen-contract-types.py）
#
# 契約を変えるときは契約PRでYAMLを直し、make gen を実行してコミットする。
# ==========================================================================
# credo:disable-for-this-file

defmodule WordWolf.Contract.Events do
  @moduledoc """
  Channel契約（asyncapi.yaml）のイベント名とpayload型。

  イベント名は定数で持ち、Channelのハンドラに文字列を直書きしない。
  openapi.yaml と同名スキーマでも内容が一致しない場合があるため
  （例: `GameResult` は REST のみ `myVote` を持つ）、独立に生成している。
  """

  # クライアント→サーバー（room チャンネル）
  @client_events ~w(comeback.draft comeback.submit room.ready chat.send discussion.skip state.request)

  # サーバー→部屋の全接続（room チャンネル）
  @server_room_events ~w(
    room.removed comeback.updated room.state phase.changed chat.published
    player.connection_changed game.paused game.resumed skip.progress
    vote.progress result.published game.aborted
  )

  # サーバー→本人のみ（user チャンネル）
  @server_user_events ~w(room.removed private.state connection.replaced friend.presence_changed room.invited)

  @spec client_events() :: [String.t()]
  def client_events, do: @client_events

  @spec server_room_events() :: [String.t()]
  def server_room_events, do: @server_room_events

  @spec server_user_events() :: [String.t()]
  def server_user_events, do: @server_user_events

  @spec server_events() :: [String.t()]
  def server_events, do: @server_room_events ++ @server_user_events

  @spec all_events() :: [String.t()]
  def all_events, do: @client_events ++ server_events()

  @doc "契約に定義されたイベント名かどうか。"
  @spec known_event?(String.t()) :: boolean()
  def known_event?(name) when is_binary(name), do: name in all_events()

  # --- asyncapi.yaml の components.schemas ---

  @type room_removed :: %{
          required(:roomId) => String.t(),
          optional(:roomName) => String.t(),
          required(:reason) => :REMOVED_BY_HOST
        }

  # 一時停止は独立フェーズにせず、進行フェーズに `paused: true` を付与する
  @type phase :: :LOBBY | :DEALING | :DISCUSSION | :VOTING | :COMEBACK | :RESULT

  @spec phase_values() :: [String.t()]
  def phase_values, do: ~w(LOBBY DEALING DISCUSSION VOTING COMEBACK RESULT)

  # 選択可能なお題ジャンル。恋愛・本音と価値観は廃止し、新規作成・設定変更では指定できない。
  @type room_genre :: :PLACE | :FOOD | :HOBBY | :DAILY_ITEM | :WORK_SCHOOL | :ENTERTAINMENT

  @spec room_genre_values() :: [String.t()]
  def room_genre_values, do: ~w(PLACE FOOD HOBBY DAILY_ITEM WORK_SCHOOL ENTERTAINMENT)

  # お題ジャンル。openapi.yaml の Genre と同一語彙
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

  # 議論スキップに賛成した人数。全員（skippedCount == playerCount）で VOTING へ移る
  @type skip_progress :: %{
          required(:skippedCount) => integer(),
          required(:playerCount) => integer()
        }

  @type vote_progress :: %{
          required(:votedCount) => integer(),
          required(:playerCount) => integer()
        }

  # ロビーを含む公開状態のスナップショット。閲覧者によらず同一（秘密情報を含めない）
  @type room_state_snapshot :: %{
          required(:comeback) => comeback_state() | nil,
          required(:roomId) => String.t(),
          required(:gameId) => String.t() | nil,
          required(:phase) => phase(),
          required(:phaseVersion) => integer(),
          required(:deadline) => String.t() | nil,
          required(:paused) => boolean(),
          required(:reconnectDeadline) => String.t() | nil,
          required(:hostUserId) => String.t(),
          optional(:settings) => %{
            optional(:genres) => [room_genre()],
            optional(:comebackEnabled) => boolean(),
            required(:discussionMs) => integer(),
            required(:voteMs) => integer()
          },
          required(:players) => [%{
            required(:userId) => String.t(),
            required(:displayName) => String.t(),
            required(:avatarUrl) => String.t(),
            required(:ready) => boolean(),
            required(:connected) => boolean()
          }],
          required(:spectators) => [%{
            required(:userId) => String.t(),
            required(:displayName) => String.t(),
            required(:avatarUrl) => String.t()
          }],
          required(:skipProgress) => skip_progress() | nil,
          required(:voteProgress) => vote_progress() | nil
        }

  # openapi.yaml の GameResult と同じ公開内容（`myVote` はHTTP側のみ。個別の投票先は配信しない）
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
          required(:players) => [%{
            required(:userId) => String.t(),
            required(:displayName) => String.t(),
            required(:avatarUrl) => String.t(),
            required(:role) => :CITIZEN | :WORD_WOLF,
            required(:word) => String.t(),
            required(:votesReceived) => integer()
          }]
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

  @type room_invite :: %{
          required(:inviteId) => String.t(),
          required(:roomId) => String.t(),
          required(:inviteCode) => String.t(),
          required(:roomName) => String.t(),
          required(:from) => %{
            required(:userId) => String.t(),
            required(:displayName) => String.t(),
            required(:avatarUrl) => String.t()
          },
          required(:sentAt) => String.t()
        }

  # --- イベントpayload ---

  # comeback.draft
  @type comeback_draft :: %{
          required(:content) => String.t()
        }

  # comeback.submit
  @type comeback_submit :: %{
          required(:content) => String.t()
        }

  # room.ready（クライアント→サーバー）
  @type room_ready :: %{
          required(:ready) => boolean()
        }

  # chat.send（クライアント→サーバー）
  @type chat_send :: %{
          required(:clientMessageId) => String.t(),
          required(:content) => String.t()
        }

  # discussion.skip（クライアント→サーバー）
  @type discussion_skip :: %{
          required(:skip) => boolean()
        }

  # state.request（クライアント→サーバー）
  @type state_request :: %{}

  # room.removed → room_removed()

  # comeback.updated → comeback_state()

  # room.state → room_state_snapshot()

  # phase.changed（サーバー→全員）
  @type phase_changed :: %{
          required(:gameId) => String.t(),
          required(:phase) => phase(),
          required(:phaseVersion) => integer(),
          required(:deadline) => String.t() | nil
        }

  # chat.published（サーバー→全員）
  @type chat_published :: %{
          required(:messageId) => String.t(),
          required(:clientMessageId) => String.t(),
          required(:userId) => String.t(),
          required(:displayName) => String.t(),
          required(:content) => String.t(),
          required(:sentAt) => String.t()
        }

  # player.connection_changed（サーバー→全員）
  @type player_connection_changed :: %{
          required(:userId) => String.t(),
          required(:connected) => boolean()
        }

  # game.paused（サーバー→全員）
  @type game_paused :: %{
          required(:reason) => :PLAYER_DISCONNECTED,
          required(:disconnectedUserIds) => [String.t()],
          required(:reconnectDeadline) => String.t()
        }

  # game.resumed（サーバー→全員）
  @type game_resumed :: %{
          required(:phase) => phase(),
          required(:phaseVersion) => integer(),
          required(:deadline) => String.t()
        }

  # skip.progress → skip_progress()

  # vote.progress → vote_progress()

  # result.published → game_result()

  # game.aborted（サーバー→全員）
  @type game_aborted :: %{
          required(:gameId) => String.t(),
          required(:reason) => :RECONNECT_TIMEOUT | :INTERNAL_ERROR,
          required(:nextPhase) => :LOBBY
        }

  # room.removed → room_removed()

  # private.state（サーバー→本人）
  @type private_state :: %{
          required(:gameId) => String.t(),
          required(:word) => String.t(),
          required(:category) => genre()
        }

  # connection.replaced（サーバー→古い接続の本人）
  @type connection_replaced :: %{
          required(:reason) => :NEWER_CONNECTION
        }

  # friend.presence_changed（サーバー→本人）
  @type friend_presence_changed :: %{
          required(:userId) => String.t(),
          required(:state) => :ONLINE | :IN_GAME | :OFFLINE
        }

  # room.invited → room_invite()

  # --- イベント名 → payload型の対応 ---
  #
  #   comeback.draft             comeback_draft()
  #   comeback.submit            comeback_submit()
  #   room.ready                 room_ready()
  #   chat.send                  chat_send()
  #   discussion.skip            discussion_skip()
  #   state.request              state_request()
  #   room.removed               room_removed()
  #   comeback.updated           comeback_state()
  #   room.state                 room_state_snapshot()
  #   phase.changed              phase_changed()
  #   chat.published             chat_published()
  #   player.connection_changed  player_connection_changed()
  #   game.paused                game_paused()
  #   game.resumed               game_resumed()
  #   skip.progress              skip_progress()
  #   vote.progress              vote_progress()
  #   result.published           game_result()
  #   game.aborted               game_aborted()
  #   room.removed               room_removed()
  #   private.state              private_state()
  #   connection.replaced        connection_replaced()
  #   friend.presence_changed    friend_presence_changed()
  #   room.invited               room_invite()

end
