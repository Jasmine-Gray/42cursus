defmodule WordWolf.Repo.Migrations.CreateMvpTables do
  use Ecto.Migration

  def change do
    create table(:users, primary_key: false) do
      add :id, :string, primary_key: true
      add :email, :string, null: false
      add :password_hash, :string, null: false
      add :display_name, :string, null: false
      add :avatar_url, :string, null: false
      add :bio, :text
      add :presence_state, :string, null: false, default: "OFFLINE"
      add :last_seen_at, :utc_datetime_usec
      add :terms_accepted_at, :utc_datetime_usec, null: false

      timestamps(type: :utc_datetime_usec)
    end

    create unique_index(:users, [:email])

    create constraint(:users, :users_presence_state_check,
             check: "presence_state IN ('ONLINE', 'IN_GAME', 'OFFLINE')"
           )

    create table(:sessions, primary_key: false) do
      add :id, :string, primary_key: true

      add :user_id, references(:users, type: :string, on_delete: :delete_all), null: false

      add :token_hash, :string, null: false
      add :expires_at, :utc_datetime_usec, null: false
      add :last_used_at, :utc_datetime_usec

      timestamps(type: :utc_datetime_usec, updated_at: false)
    end

    create index(:sessions, [:user_id])
    create unique_index(:sessions, [:token_hash])

    create table(:friendships, primary_key: false) do
      add :id, :string, primary_key: true

      add :requester_id, references(:users, type: :string, on_delete: :delete_all), null: false

      add :addressee_id, references(:users, type: :string, on_delete: :delete_all), null: false

      add :status, :string, null: false, default: "PENDING"
      add :responded_at, :utc_datetime_usec

      timestamps(type: :utc_datetime_usec)
    end

    create unique_index(:friendships, [:requester_id, :addressee_id])
    create index(:friendships, [:addressee_id, :status])

    create constraint(:friendships, :friendships_different_users_check,
             check: "requester_id <> addressee_id"
           )

    create constraint(:friendships, :friendships_status_check,
             check: "status IN ('PENDING', 'ACCEPTED', 'DECLINED')"
           )

    create table(:rooms, primary_key: false) do
      add :id, :string, primary_key: true

      add :host_user_id, references(:users, type: :string, on_delete: :delete_all), null: false

      add :room_name, :string, null: false
      add :invite_code, :string, null: false
      add :status, :string, null: false, default: "WAITING"
      add :genres, {:array, :string}, null: false
      add :word_proximity, :string, null: false, default: "NORMAL"
      add :discussion_ms, :integer, null: false, default: 300_000
      add :vote_ms, :integer, null: false, default: 30_000
      add :min_players, :integer, null: false, default: 4
      add :max_players, :integer, null: false, default: 8
      add :reconnect_grace_ms, :integer, null: false, default: 20_000
      add :closed_at, :utc_datetime_usec

      timestamps(type: :utc_datetime_usec)
    end

    create unique_index(:rooms, [:invite_code])
    create index(:rooms, [:host_user_id])

    create constraint(:rooms, :rooms_status_check,
             check: "status IN ('WAITING', 'IN_GAME', 'CLOSED')"
           )

    create constraint(:rooms, :rooms_word_proximity_check,
             check: "word_proximity IN ('EASY', 'NORMAL', 'HARD')"
           )

    create constraint(:rooms, :rooms_genres_not_empty_check, check: "cardinality(genres) > 0")

    create constraint(:rooms, :rooms_player_range_check,
             check: "min_players >= 4 AND max_players <= 8 AND min_players <= max_players"
           )

    create table(:room_members, primary_key: false) do
      add :id, :string, primary_key: true

      add :room_id, references(:rooms, type: :string, on_delete: :delete_all), null: false

      add :user_id, references(:users, type: :string, on_delete: :delete_all), null: false

      add :participant_type, :string, null: false
      add :ready, :boolean, null: false, default: false
      add :connected, :boolean, null: false, default: true
      add :joined_at, :utc_datetime_usec, null: false

      timestamps(type: :utc_datetime_usec)
    end

    create unique_index(:room_members, [:room_id, :user_id])
    create index(:room_members, [:user_id])

    create constraint(:room_members, :room_members_participant_type_check,
             check: "participant_type IN ('PLAYER', 'SPECTATOR')"
           )

    create constraint(:room_members, :room_members_spectator_not_ready_check,
             check: "participant_type = 'PLAYER' OR ready = false"
           )

    create table(:word_pairs, primary_key: false) do
      add :id, :string, primary_key: true
      add :genre, :string, null: false
      add :word_proximity, :string, null: false
      add :majority_word, :string, null: false
      add :minority_word, :string, null: false
      add :active, :boolean, null: false, default: true

      timestamps(type: :utc_datetime_usec)
    end

    create unique_index(
             :word_pairs,
             [:genre, :word_proximity, :majority_word, :minority_word],
             name: :word_pairs_unique_pair_index
           )

    create constraint(:word_pairs, :word_pairs_genre_check,
             check:
               "genre IN ('PLACE', 'FOOD', 'HOBBY', 'DAILY_ITEM', 'WORK_SCHOOL', 'ENTERTAINMENT')"
           )

    create constraint(:word_pairs, :word_pairs_word_proximity_check,
             check: "word_proximity IN ('EASY', 'NORMAL', 'HARD')"
           )

    create constraint(:word_pairs, :word_pairs_different_words_check,
             check: "majority_word <> minority_word"
           )

    create table(:games, primary_key: false) do
      add :id, :string, primary_key: true

      add :room_id, references(:rooms, type: :string, on_delete: :delete_all), null: false

      add :word_pair_id, references(:word_pairs, type: :string, on_delete: :nothing), null: false

      add :phase, :string, null: false
      add :phase_version, :integer, null: false, default: 0
      add :deadline, :utc_datetime_usec
      add :paused, :boolean, null: false, default: false
      add :reconnect_deadline, :utc_datetime_usec
      add :winner, :string
      add :result_reason, :string
      add :started_at, :utc_datetime_usec, null: false
      add :ended_at, :utc_datetime_usec
      add :aborted_at, :utc_datetime_usec

      timestamps(type: :utc_datetime_usec)
    end

    create index(:games, [:room_id, :started_at])

    create constraint(:games, :games_phase_check,
             check: "phase IN ('DEALING', 'DISCUSSION', 'VOTING', 'RESULT')"
           )

    create constraint(:games, :games_phase_version_check, check: "phase_version >= 0")

    create constraint(:games, :games_winner_check,
             check: "winner IS NULL OR winner IN ('CITIZEN_SIDE', 'WOLF_SIDE')"
           )

    create constraint(:games, :games_result_reason_check,
             check:
               "result_reason IS NULL OR result_reason IN ('WOLF_IDENTIFIED', 'WOLF_ESCAPED')"
           )

    create table(:game_players, primary_key: false) do
      add :id, :string, primary_key: true

      add :game_id, references(:games, type: :string, on_delete: :delete_all), null: false

      add :user_id, references(:users, type: :string, on_delete: :delete_all), null: false

      add :participant_type, :string, null: false
      add :role, :string
      add :assigned_word, :string
      add :connected, :boolean, null: false, default: true

      timestamps(type: :utc_datetime_usec)
    end

    create unique_index(:game_players, [:game_id, :user_id])
    create index(:game_players, [:user_id, :game_id])

    create constraint(:game_players, :game_players_participant_type_check,
             check: "participant_type IN ('PLAYER', 'SPECTATOR')"
           )

    create constraint(:game_players, :game_players_role_check,
             check: "role IS NULL OR role IN ('CITIZEN', 'WORD_WOLF')"
           )

    create constraint(:game_players, :game_players_assignment_check,
             check:
               "(participant_type = 'PLAYER' AND role IS NOT NULL AND assigned_word IS NOT NULL) OR " <>
                 "(participant_type = 'SPECTATOR' AND role IS NULL AND assigned_word IS NULL)"
           )

    create table(:chat_messages, primary_key: false) do
      add :id, :string, primary_key: true

      add :game_id, references(:games, type: :string, on_delete: :delete_all), null: false

      add :user_id, references(:users, type: :string, on_delete: :delete_all), null: false

      add :content, :text, null: false
      add :sent_at, :utc_datetime_usec, null: false

      timestamps(type: :utc_datetime_usec, updated_at: false)
    end

    create index(:chat_messages, [:game_id, :sent_at])

    create constraint(:chat_messages, :chat_messages_content_not_blank_check,
             check: "char_length(btrim(content)) > 0"
           )

    create table(:votes, primary_key: false) do
      add :id, :string, primary_key: true

      add :game_id, references(:games, type: :string, on_delete: :delete_all), null: false

      add :voter_user_id, references(:users, type: :string, on_delete: :delete_all), null: false

      add :target_user_id, references(:users, type: :string, on_delete: :delete_all), null: false

      add :voted_at, :utc_datetime_usec, null: false

      timestamps(type: :utc_datetime_usec, updated_at: false)
    end

    create unique_index(:votes, [:game_id, :voter_user_id])
    create index(:votes, [:game_id, :target_user_id])

    create constraint(:votes, :votes_not_self_check, check: "voter_user_id <> target_user_id")
  end
end
