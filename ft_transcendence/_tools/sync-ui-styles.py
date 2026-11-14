# ==============================================================================
#  _tools/sync-ui-styles.py
# ------------------------------------------------------------------------------
#  Created : 2026-09-12  yayoi111 (d/character-ui-cleanup) 共通CSSの同期と検証
# ------------------------------------------------------------------------------
#  Updated : 2026-09-26  yayoi111 (d/mobile-style-polish) shell / game / results の更新履歴
#  役割 : モックを正本に、Dockerでも単独でビルドできるFE内へCSSを同期する
#  関連 : _docs/mock/src/index.css, _docs/mock/src/styles/, frontend/src/styles/
# ==============================================================================

import argparse
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SOURCE = ROOT / "_docs/mock/src"
TARGET = ROOT / "frontend/src"
MARKER = "同期元は _docs/mock/src/。make ui-styles で更新する。"


def frontend_header(relative: Path) -> str:
    updated = {
        "styles/game.css": "2026-09-26  yayoi111 (d/mobile-style-polish) スマホで残り時間がフェーズ名に重なるのを修正",
        "styles/accessibility.css": "2026-09-13  yayoi111 (d/character-ui-cleanup) 動画の静止画対応と旧指定の整理",
        "styles/controls.css": "2026-09-13  yayoi111 (d/character-ui-cleanup) ホーム基準の共通ボタン",
        "styles/home.css": "2026-09-13  yayoi111 (d/character-ui-cleanup) ボタン配色を共通化",
        "styles/rooms.css": "2026-09-13  yayoi111 (d/character-ui-cleanup) 参加・作成のレイアウト統一",
        "styles/shell.css": "2026-09-26  yayoi111 (d/mobile-style-polish) スマホの左右余白と上部メニュー4項目の1行化",
        "styles/results.css": "2026-09-26  yayoi111 (d/mobile-style-polish) スマホで開票の列が重なるのを修正",
    }.get(relative.as_posix(), "2026-09-12  yayoi111 (d/character-ui-cleanup) 重複・未使用CSS整理")
    created = (
        "2026-08-21  yayoi111 共通CSSの入口"
        if relative == Path("index.css")
        else "2026-09-12  yayoi111 (d/character-ui-cleanup) CSSを役割ごとに分割"
    )
    return f"""/*
 * =============================================================================
 *  frontend/src/{relative.as_posix()}
 * -----------------------------------------------------------------------------
 *  Created : {created}
 *  Authors : 2026-08-29  hanabanana111 (d/home-page) 元のホームUI
 *            2026-08-31  otnk666 (b/login-register) 元の共通UI
 *            2026-09-02  Jasmine-Gray (d/layered-bg-animation) 元の背景レイヤー
 *  Updated : {updated}
 * -----------------------------------------------------------------------------
 *  {MARKER}
 *  モック : _docs/mock/src/{relative.as_posix()}
 *  関連   : _tools/sync-ui-styles.py
 *  所有   : レーンD
 * =============================================================================
 */

"""


def main() -> int:
    parser = argparse.ArgumentParser(description="モックの共通CSSをFEへ同期")
    parser.add_argument("--check", action="store_true", help="変更せず差分を検証")
    args = parser.parse_args()
    sources = [SOURCE / "index.css", *sorted((SOURCE / "styles").glob("*.css"))]
    expected = set()
    changed = []
    for source in sources:
        relative = source.relative_to(SOURCE)
        destination = TARGET / relative
        expected.add(destination)
        content = frontend_header(relative) + source.read_text()
        if destination.exists() and destination.read_text() == content:
            continue
        changed.append(destination.relative_to(ROOT).as_posix())
        if not args.check:
            destination.parent.mkdir(parents=True, exist_ok=True)
            destination.write_text(content)

    # Only remove obsolete files owned by this tool; leave other CSS untouched.
    for destination in (TARGET / "styles").glob("*.css"):
        if destination not in expected and MARKER in destination.read_text():
            changed.append(destination.relative_to(ROOT).as_posix())
            if not args.check:
                destination.unlink()

    if args.check and changed:
        print("CSSが同期されていません。make ui-styles を実行してください。")
        print("\n".join(changed))
        return 1
    print(f"CSS同期: {len(changed)}件更新" if not args.check else "CSSは正本と一致しています。")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
