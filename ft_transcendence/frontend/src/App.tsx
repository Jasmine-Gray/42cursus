// ルーティングは _docs/mock/src/App.tsx が移植元。実装済みの画面から順に Route を足していく。
// 01ログイン/02登録/規約（レーンB）、03アバター設定/22プロフィール（レーンC）、04ホーム（レーンD）。
import {
  GameDiscussionPage,
  PausedGamePage,
  ResultPage,
  SpectatorVotePage,
  VotePage,
  WordRevealPage,
} from '@/mocks/pages/game-pages'
import { LobbyPage as MockLobbyPage } from '@/mocks/pages/home-pages'
import { GameCountdownPage } from '@/mocks/pages/game-countdown-page'
import { Route, Switch } from 'wouter'

import { AuthGate } from '@/components/layout/auth-gate'
import { MobileShell } from '@/components/layout/mobile-shell'
import { AvatarSetupPage } from '@/pages/avatar-setup-page'
import { DevLoginPage } from '@/pages/dev-login-page'
import { FriendsPage } from '@/pages/friends-page'
import { GamePage } from '@/pages/game-page'
import { HomePage } from '@/pages/home-page'
import { PrivacyPage, TermsPage } from '@/pages/legal-pages'
import { LobbyPage } from '@/pages/lobby-page'
import { LoginPage } from '@/pages/login-page'
import { ProfilePage } from '@/pages/profile-page'
import { RegisterPage } from '@/pages/register-page'
import { RoomCreatePage } from '@/pages/room-create-page'
import { RoomSearchPage } from '@/pages/room-search-page'

export function App() {
  return (
    <AuthGate>
      <Switch>
        <Route path="/" component={LoginPage} />
        <Route path="/login" component={LoginPage} />
        <Route path="/register" component={RegisterPage} />
        <Route path="/avatar/setup" component={AvatarSetupPage} />
        <Route path="/home" component={HomePage} />
        <Route path="/rooms" component={RoomSearchPage} />
        <Route path="/rooms/new" component={RoomCreatePage} />
        <Route path="/join/:code" component={RoomSearchPage} />
        <Route path="/lobby" component={LobbyPage} />

        {/*
        実データのゲーム画面。カウントダウン・お題確認・議論・投票・結果を1つのコンテナが
        担い、どれを出すかはサーバーの phase.changed（room.state.phase）が決める。
        クライアント側でフェーズごとのURLへ遷移させない（契約 asyncapi.yaml）。
      */}
        <Route path="/game" component={GamePage} />

        {/*
        デザイン確認用のモック（固定データ）。実装の進捗に関係なく画面を見られるように
        /mock/* に隔離してある。実データ版は上の /game と /lobby。
      */}
        <Route path="/mock/lobby/player">
          <MockLobbyPage mode="player" viewpoint="citizen" />
        </Route>
        <Route path="/mock/lobby/spectator">
          <MockLobbyPage mode="spectator" viewpoint="spectator" />
        </Route>
        <Route path="/mock/game/countdown">
          <GameCountdownPage viewpoint="citizen" />
        </Route>
        <Route path="/mock/game/countdown/spectator">
          <GameCountdownPage viewpoint="spectator" />
        </Route>
        <Route path="/mock/game/reveal">
          <WordRevealPage viewpoint="citizen" />
        </Route>
        <Route path="/mock/game">
          <GameDiscussionPage viewpoint="citizen" />
        </Route>
        <Route path="/mock/game/spectator">
          <GameDiscussionPage viewpoint="spectator" />
        </Route>
        <Route path="/mock/game/spectator/vote" component={SpectatorVotePage} />
        <Route path="/mock/game/paused" component={PausedGamePage} />
        <Route path="/mock/game/vote" component={VotePage} />
        <Route path="/mock/game/result" component={ResultPage} />
        <Route path="/friends" component={FriendsPage} />
        <Route path="/profile" component={ProfilePage} />
        <Route path="/privacy" component={PrivacyPage} />
        <Route path="/terms" component={TermsPage} />
        {/* 開発時のみ: 同じブラウザの別タブを別のシードユーザーにする（複数人でロビー・ゲームを試す用） */}
        {import.meta.env.DEV && (
          <Route path="/dev/as/:name?" component={DevLoginPage} />
        )}
        <Route>
          <NotFoundPage />
        </Route>
      </Switch>
    </AuthGate>
  )
}

function NotFoundPage() {
  return (
    <MobileShell title="画面が見つかりません" backHref="/home">
      <main className="px-6 py-16 text-center">
        <p className="font-mono text-4xl font-semibold tabular-nums">404</p>
        <h2 className="mt-4 text-xl font-semibold text-balance">
          ページが見つかりません
        </h2>
      </main>
    </MobileShell>
  )
}
