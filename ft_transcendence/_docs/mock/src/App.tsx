import { lazy, Suspense } from 'react'
import { Route, Switch } from 'wouter'

import { MobileShell } from '@/components/layout/mobile-shell'
import { LoginPage, RegisterPage } from '@/pages/auth-pages'
import { GameDiscussionPage, PausedGamePage, ResultPage, SpectatorVotePage, VotePage, WordRevealPage } from '@/pages/game-pages'
import { CreateRoomPage, HomePage, LobbyPage, RoomsPage } from '@/pages/home-pages'
import { PrivacyPage, TermsPage } from '@/pages/legal-pages'
import { AvatarSetupPage, FriendsPage, ProfilePage } from '@/pages/social-pages'

const GameCountdownPage = lazy(() => import('@/pages/game-countdown-page'))

function App() {
  return (
    <Switch>
      <Route path="/" component={LoginPage} />
      <Route path="/login" component={LoginPage} />
      <Route path="/register" component={RegisterPage} />
      <Route path="/avatar/setup" component={AvatarSetupPage} />
      <Route path="/home" component={HomePage} />
      <Route path="/rooms" component={RoomsPage} />
      <Route path="/rooms/new" component={CreateRoomPage} />
      <Route path="/join/:code" component={RoomsPage} />
      <Route path="/lobby/player"><LobbyPage mode="player" viewpoint="citizen" /></Route>
      <Route path="/lobby/spectator"><LobbyPage mode="spectator" viewpoint="spectator" /></Route>
      <Route path="/lobby"><LobbyPage mode="host" viewpoint="citizen" /></Route>
      <Route path="/game/countdown"><Suspense fallback={null}><GameCountdownPage viewpoint="citizen" /></Suspense></Route>
      <Route path="/game/countdown/spectator"><Suspense fallback={null}><GameCountdownPage viewpoint="spectator" /></Suspense></Route>
      <Route path="/game/reveal"><WordRevealPage viewpoint="citizen" /></Route>
      <Route path="/game"><GameDiscussionPage viewpoint="citizen" /></Route>
      <Route path="/game/spectator"><GameDiscussionPage viewpoint="spectator" /></Route>
      <Route path="/game/spectator/vote" component={SpectatorVotePage} />
      <Route path="/game/paused" component={PausedGamePage} />
      <Route path="/game/vote" component={VotePage} />
      <Route path="/game/result" component={ResultPage} />
      <Route path="/friends" component={FriendsPage} />
      <Route path="/profile" component={ProfilePage} />
      <Route path="/privacy" component={PrivacyPage} />
      <Route path="/terms" component={TermsPage} />
      <Route><NotFoundPage /></Route>
    </Switch>
  )
}

function NotFoundPage() {
  return (
    <MobileShell title="画面が見つかりません" backHref="/home">
      <main className="px-6 py-16 text-center">
        <p className="font-mono text-4xl font-semibold tabular-nums">404</p>
        <h2 className="mt-4 text-balance text-xl font-semibold">ページが見つかりません</h2>
        <p className="mt-2 text-pretty text-sm text-muted-foreground">ホームから移動してください。</p>
      </main>
    </MobileShell>
  )
}

export default App
