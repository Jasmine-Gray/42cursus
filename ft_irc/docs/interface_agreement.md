# Interface Agreement

メンバー間の境界APIと未解決事項をまとめる。
変更時は全員に共有すること。

---

## Member A ↔ Member B

### A が B に提供するもの

| API | 場所 | 状態 |
|-----|------|------|
| `ClientSession::appendInput()` | `ClientSession.hpp` | スタブあり |
| `ClientSession::popNextLine()` | `ClientSession.hpp` | スタブあり |
| `ClientSession::enqueueReply()` | `ClientSession.hpp` | スタブあり |
| `ClientSession::getFd()` | `ClientSession.hpp` | スタブあり |
| `Server::dispatchLineForTest()` | `Server.hpp` | テスト用スタブあり |

### B が A に要求するもの（未実装）

| 必要なAPI | 用途 | 優先度 |
|----------|------|--------|
| `Server::findClientByNick(nick)` | PRIVMSG ユーザー宛て転送 | M2 |
| `Server::broadcastToChannel(channel, msg)` または ChannelManager経由 | JOIN/KICK等のブロードキャスト | M2 |

### 未解決事項
- `findClientByNick()` は `Server` に置くか `ChannelManager` 経由にするか → **要相談**
- `ClientSession` のポインタ管理方針（raw pointer か reference か）→ **要相談**

---

## Member B ↔ Member C

### C が B に提供するもの

| API | 場所 | 状態 |
|-----|------|------|
| `ChannelManager::join()` | `ChannelManager.hpp` | スタブあり |
| `ChannelManager::part()` | `ChannelManager.hpp` | スタブあり |
| `ChannelManager::sendToChannel()` | `ChannelManager.hpp` | スタブあり |
| `ChannelManager::kick()` | `ChannelManager.hpp` | スタブあり |
| `ChannelManager::invite()` | `ChannelManager.hpp` | スタブあり |
| `ChannelManager::setTopic()` | `ChannelManager.hpp` | スタブあり |
| `ChannelManager::setMode()` | `ChannelManager.hpp` | スタブあり |
| `ChannelManager::isMember()` | `ChannelManager.hpp` | スタブあり |

### B が C に要求するもの（未実装）

| 必要なAPI | 用途 | 優先度 |
|----------|------|--------|
| `ChannelManager::getMembers(channel)` | JOIN成功時のRPL_NAMREPLY | M2 |
| `ChannelManager::getTopic(channel)` | JOIN成功時のRPL_TOPIC | M2 |
| `ChannelManager::isOperator(channel, nick)` | KICK/INVITE/TOPIC/MODEの権限チェック | M3 |
| `ChannelManager::getMode(channel)` | MODEの現在値取得 | M3 |

### 現状の ChannelManager の問題点
- `join()` / `kick()` 等が `bool` しか返さない
- エラーの種類が判別できない（例：チャンネルなし vs オペレーターでない vs 満員）
- **B が正確な numeric reply を返すには、エラー種別が必要** → C と要相談

### 未解決事項
- `ChannelManager` のエラー返却方法（bool → enum か error code に変更するか）→ **要相談**
- numeric reply 定数の管理場所（共通ヘッダー？）→ **要相談**
- RPL_NAMREPLY の `@` prefix（オペレーター表示）はどこで付与するか → **要相談**

---

## 全員共通

### 現状のスタブで動くテスト方法
```cpp
// Server::dispatchLineForTest() を使う
ClientSession client(0);
server.dispatchLineForTest(client, "PASS password\r\n");
server.dispatchLineForTest(client, "NICK alice\r\n");
server.dispatchLineForTest(client, "USER alice 0 * :Alice\r\n");
```

### ブロードキャストの責任分担（未確定）
チャンネルへのブロードキャスト（JOIN通知、KICK通知等）は誰が実装するか未確定。

| 案 | 説明 |
|----|------|
| A案 | ChannelManager がメンバーのfdリストを持ち、自前でブロードキャスト |
| B案 | ChannelManager がnickリストを返し、CommandDispatcher が各ClientSessionに送る |
| C案 | Server がブロードキャスト関数を持つ |

→ **M2開始前に全員で決定が必要**
