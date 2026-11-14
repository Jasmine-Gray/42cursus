defmodule WordWolfWeb.Router do
  use WordWolfWeb, :router

  # 認証不要（契約で security: [] のもの）
  pipeline :api do
    plug :accepts, ["json"]
  end

  # 要認証（Bearer トークン必須。無効時は 401）
  pipeline :authenticated do
    plug :accepts, ["json"]
    plug WordWolfWeb.Plugs.Authenticate
  end

  # 契約（_docs/spec/api/openapi.yaml）のパスには /api prefix が無いため、scope は "/"
  scope "/", WordWolfWeb do
    pipe_through :api

    get "/health", HealthController, :show

    post "/auth/register", AuthController, :register
    post "/auth/login", AuthController, :login
  end

  scope "/", WordWolfWeb do
    pipe_through :authenticated

    post "/auth/logout", AuthController, :logout

    get "/users", UserController, :index
    get "/users/me", UserController, :show
    put "/users/me", UserController, :update
    delete "/users/me", UserController, :delete
    put "/users/me/avatar", UserController, :update_avatar
    get "/users/me/games", UserController, :games
    get "/users/:userId", UserController, :profile

    get "/games/:game_id/messages", GameController, :messages
    get "/games/:gameId", GameController, :show
    get "/games/:gameId/assignment", GameController, :assignment
    get "/games/:gameId/spectator-assignments", GameController, :spectator_assignments
    post "/games/:gameId/votes", GameController, :vote
    get "/games/:gameId/result", GameController, :result

    get "/friends", FriendController, :index
    delete "/friends/:userId", FriendController, :delete
    get "/friends/requests", FriendController, :list_requests
    post "/friends/requests", FriendController, :create_request
    post "/friends/requests/:requestId/accept", FriendController, :accept_request
    post "/friends/requests/:requestId/decline", FriendController, :decline_request

    post "/rooms", RoomController, :create
    get "/rooms/:roomId", RoomController, :show
    put "/rooms/:roomId/settings", RoomController, :update_settings
    delete "/rooms/:roomId/members/:userId", RoomController, :remove_member
    post "/rooms/:roomId/join", RoomController, :join
    post "/rooms/:roomId/leave", RoomController, :leave
    post "/rooms/:roomId/start", RoomController, :start
    post "/rooms/:roomId/invites", RoomController, :invite
    get "/invites", RoomController, :list_invites
    get "/invites/:inviteCode", RoomController, :show_invite
    post "/invites/:inviteId/accept", RoomController, :accept_invite
    post "/invites/:inviteId/decline", RoomController, :decline_invite
  end
end
