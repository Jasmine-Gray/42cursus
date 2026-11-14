defmodule HelloWebWeb.CounterLive do
  # 修正ポイント: HelloWeb ではなく HelloWebWeb にする
  use HelloWebWeb, :live_view

  # 初期状態のセット（valを0にする）
  def mount(_params, _session, socket) do
    {:ok, assign(socket, val: 0)}
  end

  # 画面の描画処理（HTML）
  def render(assigns) do
    ~H"""
    <div style="text-align: center; margin-top: 50px;">
      <h1>カウンター: <%= @val %></h1>
      
      <!-- phx-clickでボタンを押したときのアクションを指定 -->
      <button phx-click="inc" style="font-size: 20px; margin: 10px;">増やす</button>
      <button phx-click="dec" style="font-size: 20px; margin: 10px;">減らす</button>
    </div>
    """
  end

  # 「増やす（inc）」ボタンが押されたときの処理
  def handle_event("inc", _value, socket) do
    {:noreply, update(socket, :val, fn val -> val + 1 end)}
  end

  # 「減らす（dec）」ボタンが押されたときの処理
  def handle_event("dec", _value, socket) do
    {:noreply, update(socket, :val, fn val -> val - 1 end)}
  end
end
