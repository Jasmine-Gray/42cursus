defmodule WordWolf.Repo.Migrations.CreateRoomInvites do
  use Ecto.Migration

  def change do
    create table(:room_invites, primary_key: false) do
      add :id, :string, primary_key: true

      add :room_id, references(:rooms, type: :string, on_delete: :delete_all), null: false
      add :inviter_id, references(:users, type: :string, on_delete: :delete_all), null: false
      add :invitee_id, references(:users, type: :string, on_delete: :delete_all), null: false

      add :status, :string, null: false, default: "PENDING"
      add :sent_at, :utc_datetime_usec, null: false
      add :responded_at, :utc_datetime_usec

      timestamps(type: :utc_datetime_usec)
    end

    create index(:room_invites, [:invitee_id, :status])
    create index(:room_invites, [:room_id, :invitee_id])

    create constraint(:room_invites, :room_invites_status_check,
             check: "status IN ('PENDING', 'ACCEPTED', 'DECLINED')"
           )
  end
end
