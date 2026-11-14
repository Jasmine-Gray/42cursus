defmodule WordWolf.Friends do
  @moduledoc """
  フレンドのコンテキスト（契約: `_docs/spec/api/openapi.yaml` の `friends` タグ）。

  - フレンド一覧取得（オンライン状態スナップショット付き）
  - 未処理のフレンド申請一覧取得（受信または送信）
  - フレンド申請送信
  - フレンド申請の承認・拒否
  - フレンド解除
  """

  import Ecto.Query, warn: false

  alias WordWolf.Accounts
  alias WordWolf.Accounts.User
  alias WordWolf.Contract.Types
  alias WordWolf.Friends.Friendship
  alias WordWolf.Repo

  @doc """
  承認済み（ACCEPTED）フレンドの数。

  申請者・被申請者どちらの向きでも本人が含まれる行を数える。
  """
  @spec count_accepted(String.t()) :: non_neg_integer()
  def count_accepted(user_id) when is_binary(user_id) do
    Friendship
    |> where([f], f.status == "ACCEPTED")
    |> where([f], f.requester_id == ^user_id or f.addressee_id == ^user_id)
    |> Repo.aggregate(:count, :id)
  end

  @doc """
  フレンド一覧を取得する（オンライン状態付き）。
  """
  @spec list_friends(String.t()) :: [Types.friend()]
  def list_friends(user_id) when is_binary(user_id) do
    query =
      from f in Friendship,
        where: f.status == "ACCEPTED",
        where: f.requester_id == ^user_id or f.addressee_id == ^user_id,
        preload: [:requester, :addressee],
        order_by: [desc: f.updated_at]

    query
    |> Repo.all()
    |> Enum.map(fn friendship ->
      other_user =
        if friendship.requester_id == user_id do
          friendship.addressee
        else
          friendship.requester
        end

      to_friend_entry(other_user)
    end)
  end

  @doc """
  ACCEPTEDなフレンドの user_id 一覧。`WordWolf.Presence` がオンライン状態の変化を
  配信する相手を決めるのに使う（表示用の `list_friends/1` とは戻り値の形が違う）。
  """
  @spec friend_ids(String.t()) :: [String.t()]
  def friend_ids(user_id) when is_binary(user_id) do
    query =
      from f in Friendship,
        where: f.status == "ACCEPTED",
        where: f.requester_id == ^user_id or f.addressee_id == ^user_id,
        select: {f.requester_id, f.addressee_id}

    query
    |> Repo.all()
    |> Enum.map(fn {requester_id, addressee_id} ->
      if requester_id == user_id, do: addressee_id, else: requester_id
    end)
  end

  @doc """
  未処理（PENDING）のフレンド申請一覧を取得する。
  `direction` が `"outgoing"` の場合は自分が送信した申請、`"incoming"`（既定）の場合は受信した申請を返す。
  """
  @spec list_friend_requests(String.t(), String.t()) :: [Types.friend_request()]
  def list_friend_requests(user_id, direction \\ "incoming")

  def list_friend_requests(user_id, "outgoing") when is_binary(user_id) do
    query =
      from f in Friendship,
        where: f.status == "PENDING" and f.requester_id == ^user_id,
        preload: [:requester, :addressee],
        order_by: [desc: f.inserted_at]

    query
    |> Repo.all()
    |> Enum.map(fn friendship ->
      %{
        requestId: friendship.id,
        from: Accounts.to_user_public(friendship.requester),
        to: Accounts.to_user_public(friendship.addressee),
        sentAt: DateTime.to_iso8601(friendship.inserted_at)
      }
    end)
  end

  def list_friend_requests(user_id, _incoming) when is_binary(user_id) do
    query =
      from f in Friendship,
        where: f.status == "PENDING" and f.addressee_id == ^user_id,
        preload: [:requester, :addressee],
        order_by: [desc: f.inserted_at]

    query
    |> Repo.all()
    |> Enum.map(fn friendship ->
      %{
        requestId: friendship.id,
        from: Accounts.to_user_public(friendship.requester),
        to: Accounts.to_user_public(friendship.addressee),
        sentAt: DateTime.to_iso8601(friendship.inserted_at)
      }
    end)
  end

  @doc """
  フレンド申請を送信する。
  """
  @spec send_friend_request(String.t(), String.t()) ::
          {:ok, %{requestId: String.t()}}
          | {:error, :invalid_payload | :not_found | :already_submitted}
  def send_friend_request(requester_id, target_user_id)
      when is_binary(requester_id) and is_binary(target_user_id) do
    cond do
      requester_id == target_user_id ->
        {:error, :invalid_payload}

      is_nil(Accounts.get_user(target_user_id)) ->
        {:error, :not_found}

      true ->
        # 双方向で既存の申請・フレンド関係を確認
        existing =
          Friendship
          |> where(
            [f],
            (f.requester_id == ^requester_id and f.addressee_id == ^target_user_id) or
              (f.requester_id == ^target_user_id and f.addressee_id == ^requester_id)
          )
          |> Repo.one()

        case existing do
          %Friendship{status: "ACCEPTED"} ->
            {:error, :already_submitted}

          %Friendship{status: "PENDING"} ->
            {:error, :already_submitted}

          %Friendship{} = friendship ->
            # DECLINED 状態からの再申請: requester/addressee を更新して PENDING に戻す
            friendship
            |> Ecto.Changeset.change(%{
              requester_id: requester_id,
              addressee_id: target_user_id,
              status: "PENDING",
              responded_at: nil
            })
            |> Repo.update()
            |> case do
              {:ok, updated} -> {:ok, %{requestId: updated.id}}
              {:error, _} -> {:error, :invalid_payload}
            end

          nil ->
            # 新規フレンド申請
            id = Accounts.generate_id("freq")

            %Friendship{id: id}
            |> Ecto.Changeset.change(%{
              requester_id: requester_id,
              addressee_id: target_user_id,
              status: "PENDING"
            })
            |> Repo.insert()
            |> case do
              {:ok, created} -> {:ok, %{requestId: created.id}}
              {:error, _} -> {:error, :invalid_payload}
            end
        end
    end
  end

  def send_friend_request(_requester_id, _target_user_id), do: {:error, :invalid_payload}

  @doc """
  フレンド申請を承認する。
  """
  @spec accept_friend_request(String.t(), String.t()) ::
          {:ok, Types.friend()} | {:error, :not_found}
  def accept_friend_request(current_user_id, request_id)
      when is_binary(current_user_id) and is_binary(request_id) do
    query =
      from f in Friendship,
        where: f.id == ^request_id and f.addressee_id == ^current_user_id and f.status == "PENDING",
        preload: [:requester]

    case Repo.one(query) do
      nil ->
        {:error, :not_found}

      friendship ->
        now = DateTime.utc_now()

        friendship
        |> Ecto.Changeset.change(%{status: "ACCEPTED", responded_at: now})
        |> Repo.update()
        |> case do
          {:ok, _updated} ->
            {:ok, to_friend_entry(friendship.requester)}

          {:error, _} ->
            {:error, :not_found}
        end
    end
  end

  def accept_friend_request(_current_user_id, _request_id), do: {:error, :not_found}

  @doc """
  フレンド申請を拒否する。
  """
  @spec decline_friend_request(String.t(), String.t()) :: {:ok, :declined} | {:error, :not_found}
  def decline_friend_request(current_user_id, request_id)
      when is_binary(current_user_id) and is_binary(request_id) do
    query =
      from f in Friendship,
        where: f.id == ^request_id and f.addressee_id == ^current_user_id and f.status == "PENDING"

    case Repo.one(query) do
      nil ->
        {:error, :not_found}

      friendship ->
        now = DateTime.utc_now()

        friendship
        |> Ecto.Changeset.change(%{status: "DECLINED", responded_at: now})
        |> Repo.update()
        |> case do
          {:ok, _updated} -> {:ok, :declined}
          {:error, _} -> {:error, :not_found}
        end
    end
  end

  def decline_friend_request(_current_user_id, _request_id), do: {:error, :not_found}

  @doc """
  フレンドを解除する（DELETE /friends/{userId}, removeFriend）。

  どちらが申請したかに関わらず、2人の間の承認済み（ACCEPTED）の行を消す。
  行ごと消すので、解除後は `send_friend_request/2` で新規に申請し直せる。
  フレンドでない（未承認・拒否済みを含む）・相手が存在しない場合は `:not_found`。
  """
  @spec remove_friend(String.t(), String.t()) :: :ok | {:error, :not_found}
  def remove_friend(current_user_id, friend_user_id)
      when is_binary(current_user_id) and is_binary(friend_user_id) do
    {count, _} =
      Friendship
      |> where([f], f.status == "ACCEPTED")
      |> where(
        [f],
        (f.requester_id == ^current_user_id and f.addressee_id == ^friend_user_id) or
          (f.requester_id == ^friend_user_id and f.addressee_id == ^current_user_id)
      )
      |> Repo.delete_all()

    if count > 0, do: :ok, else: {:error, :not_found}
  end

  # --- private helpers ---

  defp to_friend_entry(%User{} = user) do
    last_seen_iso =
      if user.presence_state == "OFFLINE" and user.last_seen_at do
        DateTime.to_iso8601(user.last_seen_at)
      else
        nil
      end

    %{
      user: Accounts.to_user_public(user),
      presence: %{
        state: user.presence_state || "OFFLINE",
        lastSeenAt: last_seen_iso
      }
    }
  end
end
