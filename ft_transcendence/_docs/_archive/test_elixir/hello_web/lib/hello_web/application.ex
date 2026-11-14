defmodule HelloWeb.Application do
  # See https://elixir.hexdocs.pm/Application.html
  # for more information on OTP Applications
  @moduledoc false

  use Application

  @impl true
  def start(_type, _args) do
    children = [
      HelloWebWeb.Telemetry,
      {DNSCluster, query: Application.get_env(:hello_web, :dns_cluster_query) || :ignore},
      {Phoenix.PubSub, name: HelloWeb.PubSub},
      # Start a worker by calling: HelloWeb.Worker.start_link(arg)
      # {HelloWeb.Worker, arg},
      # Start to serve requests, typically the last entry
      HelloWebWeb.Endpoint
    ]

    # See https://elixir.hexdocs.pm/Supervisor.html
    # for other strategies and supported options
    opts = [strategy: :one_for_one, name: HelloWeb.Supervisor]
    Supervisor.start_link(children, opts)
  end

  # Tell Phoenix to update the endpoint configuration
  # whenever the application is updated.
  @impl true
  def config_change(changed, _new, removed) do
    HelloWebWeb.Endpoint.config_change(changed, removed)
    :ok
  end
end
