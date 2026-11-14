defmodule HelloWebWeb.PageController do
  use HelloWebWeb, :controller

  def home(conn, _params) do
    render(conn, :home)
  end
end
