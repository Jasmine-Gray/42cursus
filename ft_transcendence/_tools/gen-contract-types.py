#!/usr/bin/env python3
# ==============================================================================
#  _tools/gen-contract-types.py
# ------------------------------------------------------------------------------
#  Created : 2026-08-21  yayoi111                                   契約 → 型生成スクリプトを新規作成
# ------------------------------------------------------------------------------
#  役割    : 契約から型を生成する。openapi.yaml → backend/lib/word_wolf/contract/types.ex、asyncapi.yaml → contract/events.ex と frontend/src/types/ws/events.ts
#  備考    : REST の TS 型は openapi-typescript（Makefile の gen-frontend）。生成物は手編集禁止（make gen-check で検出）
#  契約    : _docs/spec/api/openapi.yaml, _docs/spec/api/asyncapi.yaml
#  関連    : Makefile（gen / gen-check）, backend/lib/word_wolf/contract/, frontend/src/types/
# ==============================================================================

"""API契約から型を生成する。

入力（正本）:
  _docs/spec/api/openapi.yaml    REST契約（OpenAPI 3.1.1）
  _docs/spec/api/asyncapi.yaml   Channel契約（AsyncAPI 3.1.0）

出力（すべて手編集禁止）:
  frontend/src/types/ws/events.ts                  Channel payload型 + イベント名対応表
  backend/lib/word_wolf/contract/types.ex      REST契約のスキーマ型
  backend/lib/word_wolf/contract/events.ex     Channelイベント名 + payload型

REST側のTypeScript型は openapi-typescript が生成する（make gen-frontend）。
Channel側は AsyncAPI 3.1.0 を扱えるジェネレータが無いためここで生成する。

asyncapi側のスキーマは openapi 側と同名でも内容が一致しない
（例: GameResult は REST のみ myVote を持つ）ため、両者は独立に生成する。

依存: python3 + PyYAML（_docs/spec/api/build-docs.sh と同じ）
"""

import argparse
import os
import re
import sys

import yaml

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OPENAPI = os.path.join(ROOT, "_docs", "spec", "api", "openapi.yaml")
ASYNCAPI = os.path.join(ROOT, "_docs", "spec", "api", "asyncapi.yaml")
EX_DIR = os.path.join(ROOT, "backend", "lib", "word_wolf", "contract")
TS_DIR = os.path.join(ROOT, "frontend", "src", "types", "ws")

IDENT_RE = re.compile(r"^[A-Za-z_][A-Za-z0-9_]*$")

GENERATED_BY = "make gen  （_tools/gen-contract-types.py）"


def snake(name):
    name = name.replace(".", "_").replace("-", "_")
    name = re.sub(r"(?<!^)(?=[A-Z])", "_", name)
    return re.sub(r"__+", "_", name).lower()


def pascal(name):
    return "".join(part[:1].upper() + part[1:] for part in snake(name).split("_") if part)


def first_line(text, limit=104):
    if not text:
        return None
    line = str(text).strip().splitlines()[0].strip()
    return (line[: limit - 1] + "…") if len(line) > limit else (line or None)


def ref_name(ref):
    return ref.rsplit("/", 1)[-1]


def merge_all_of(members, schemas):
    merged = {"type": "object", "properties": {}, "required": []}
    for member in members:
        if "$ref" in member:
            member = schemas.get(ref_name(member["$ref"]), {})
        if "allOf" in member:
            member = merge_all_of(member["allOf"], schemas)
        merged["properties"].update(member.get("properties") or {})
        merged["required"].extend(member.get("required") or [])
    return merged


def split_nullable(schema):
    """(型名, nullableか) を返す。OpenAPI 3.1 の type: [x, "null"] に対応。"""
    kind = schema.get("type")
    if isinstance(kind, list):
        return next((k for k in kind if k != "null"), None), "null" in kind
    return kind, False


class Renderer:
    """JSON Schema を各言語の型へ変換する基底クラス。"""

    def __init__(self, schemas):
        self.schemas = schemas
        self.unresolved = []

    def render(self, schema, indent=0, path="?"):
        if not isinstance(schema, dict) or not schema:
            return self.free_map()

        if "$ref" in schema:
            return self.ref(ref_name(schema["$ref"]))

        if "allOf" in schema:
            return self.obj(merge_all_of(schema["allOf"], self.schemas), indent, path)

        members = schema.get("anyOf") or schema.get("oneOf")
        if members:
            nullable = any(m.get("type") == "null" for m in members)
            rest = [m for m in members if m.get("type") != "null"]
            if not rest:
                return self.null()
            body = self.union([self.render(m, indent, path) for m in rest])
            return self.nullable(body) if nullable else body

        kind, nullable = split_nullable(schema)

        if "const" in schema and kind == "string":
            body = self.literal(schema["const"])
        elif "enum" in schema and kind == "string":
            body = self.union([self.literal(v) for v in schema["enum"]])
        elif kind == "array":
            body = self.array(self.render(schema.get("items", {}), indent, path))
        elif kind == "object":
            body = self.obj(schema, indent, path)
        elif kind in self.PRIMITIVES:
            body = self.PRIMITIVES[kind]
        else:
            if kind is None:
                self.unresolved.append(path)
            body = self.free_map()

        return self.nullable(body) if nullable else body


class ElixirRenderer(Renderer):
    PRIMITIVES = {
        "string": "String.t()",
        "integer": "integer()",
        "number": "float()",
        "boolean": "boolean()",
    }

    def ref(self, name):
        return snake(name) + "()"

    def free_map(self):
        return "map()"

    def null(self):
        return "nil"

    def nullable(self, body):
        return body + " | nil"

    def union(self, parts):
        return " | ".join(parts)

    def array(self, inner):
        return "[%s]" % inner

    def literal(self, value):
        # 型仕様に文字列リテラルは書けないため、列挙はアトムで表現する
        return (":" + value) if IDENT_RE.match(str(value)) else "String.t()"

    def obj(self, schema, indent, path):
        props = schema.get("properties") or {}
        if not props:
            return "%{}" if schema.get("additionalProperties") is False else "map()"
        required = set(schema.get("required") or [])
        pad = " " * (indent + 2)
        entries = []
        for key, value in props.items():
            kind = "required" if key in required else "optional"
            rendered = self.render(value, indent + 2, "%s.%s" % (path, key))
            entries.append("%s%s(:%s) => %s" % (pad, kind, key, rendered))
        return "%{\n" + ",\n".join(entries) + "\n" + " " * indent + "}"


class TypeScriptRenderer(Renderer):
    PRIMITIVES = {
        "string": "string",
        "integer": "number",
        "number": "number",
        "boolean": "boolean",
    }

    def ref(self, name):
        return pascal(name)

    def free_map(self):
        return "Record<string, unknown>"

    def null(self):
        return "null"

    def nullable(self, body):
        return body + " | null"

    def union(self, parts):
        return " | ".join(parts)

    def array(self, inner):
        return ("(%s)[]" % inner) if (" " in inner or "|" in inner) else ("%s[]" % inner)

    def literal(self, value):
        return '"%s"' % value

    def obj(self, schema, indent, path):
        props = schema.get("properties") or {}
        if not props:
            return "Record<string, never>" if schema.get("additionalProperties") is False \
                else "Record<string, unknown>"
        required = set(schema.get("required") or [])
        pad = " " * (indent + 2)
        entries = []
        for key, value in props.items():
            mark = "" if key in required else "?"
            rendered = self.render(value, indent + 2, "%s.%s" % (path, key))
            note = first_line((value or {}).get("description"))
            if note:
                entries.append("%s/** %s */" % (pad, note))
            entries.append("%s%s%s: %s;" % (pad, key, mark, rendered))
        return "{\n" + "\n".join(entries) + "\n" + " " * indent + "}"


def sigil_w(values, indent, width=78):
    one_line = "~w(%s)" % " ".join(values)
    if len(indent) + len(one_line) <= 98:
        return one_line
    pad = indent + "  "
    lines, current = [], []
    for value in values:
        if current and len(pad) + len(" ".join(current + [value])) > width:
            lines.append(pad + " ".join(current))
            current = [value]
        else:
            current.append(value)
    if current:
        lines.append(pad + " ".join(current))
    return "~w(\n" + "\n".join(lines) + "\n" + indent + ")"


def ex_type_lines(name, body, indent="  "):
    head = "%s@type %s :: %s" % (indent, name, body)
    if len(head) <= 98 or body.startswith("%{") or " | " not in body:
        return [head]
    pad = " " * (len(indent) + 8)
    members = body.split(" | ")
    return (
        ["%s@type %s ::" % (indent, name), pad + members[0]]
        + ["%s| %s" % (pad, member) for member in members[1:]]
    )


def ex_header(source):
    return "\n".join(
        [
            "# " + "=" * 74,
            "# 自動生成ファイル — 手で編集しない",
            "#",
            "#   生成元: %s" % source,
            "#   生成:   %s" % GENERATED_BY,
            "#",
            "# 契約を変えるときは契約PRでYAMLを直し、make gen を実行してコミットする。",
            "# " + "=" * 74,
            "# credo:disable-for-this-file",
            "",
        ]
    )


def ts_header(source):
    return "\n".join(
        [
            "/* " + "=" * 72,
            " * 自動生成ファイル — 手で編集しない",
            " *",
            " *   生成元: %s" % source,
            " *   生成:   %s" % GENERATED_BY,
            " *",
            " * 契約を変えるときは契約PRでYAMLを直し、make gen を実行してコミットする。",
            " * " + "=" * 72 + " */",
            "",
        ]
    )


def channel_events(asyncapi):
    """operations から (イベント名, メッセージキー) を方向別に集める。"""
    messages = asyncapi["components"]["messages"]
    client, room, user = [], [], []
    for operation in asyncapi["operations"].values():
        parts = operation["messages"][0]["$ref"].split("/")
        channel, key = parts[2], parts[-1]
        event = messages[key]["name"]
        if operation["action"] == "receive":
            client.append((event, key))
        elif channel == "room":
            room.append((event, key))
        else:
            user.append((event, key))
    return client, room, user


def message_types(asyncapi, name_fn, suffix):
    """イベント名 → (型名, 宣言が必要か)。

    payloadが純粋な $ref のときは既存スキーマ型を指すだけにする
    （`vote.progress` の payload は `VoteProgress` そのものなので再宣言すると衝突する）。
    """
    schemas = asyncapi["components"].get("schemas", {})
    messages = asyncapi["components"]["messages"]
    taken = {name_fn(name) for name in schemas}
    mapping, renamed = {}, []
    for event, key in sum(channel_events(asyncapi), []):
        payload = messages[key].get("payload") or {}
        if list(payload.keys()) == ["$ref"]:
            mapping[event] = (name_fn(ref_name(payload["$ref"])), False)
            continue
        name = name_fn(event)
        if name in taken:
            renamed.append(event)
            name += suffix
        taken.add(name)
        mapping[event] = (name, True)
    return mapping, renamed


def build_types_ex(openapi):
    schemas = openapi["components"]["schemas"]
    renderer = ElixirRenderer(schemas)
    out = [ex_header("_docs/spec/api/openapi.yaml")]
    out.append("defmodule WordWolf.Contract.Types do")
    out.append('  @moduledoc """')
    out.append("  REST契約（openapi.yaml）のスキーマに対応する型仕様。")
    out.append("")
    out.append("  キーはJSONと同じcamelCaseのアトム。日時とIDは `String.t()`。")
    out.append("  列挙は `*_values/0` で文字列一覧を取得できる（Ecto.Enum や検証で使う）。")
    out.append('  """')
    out.append("")
    for name, schema in schemas.items():
        note = first_line(schema.get("description"))
        if note:
            out.append("  # %s" % note)
        out.extend(ex_type_lines(snake(name), renderer.render(schema, 8, name)))
        if "enum" in schema and schema.get("type") == "string":
            literal = sigil_w(schema["enum"], "    ")
            out.append("")
            out.append("  @spec %s_values() :: [String.t()]" % snake(name))
            if "\n" in literal:
                out.append("  def %s_values do" % snake(name))
                out.append("    %s" % literal)
                out.append("  end")
            else:
                out.append("  def %s_values, do: %s" % (snake(name), literal))
        out.append("")
    out.append("end")
    return "\n".join(out).rstrip() + "\n", renderer


def build_events_ex(asyncapi):
    schemas = asyncapi["components"].get("schemas", {})
    messages = asyncapi["components"]["messages"]
    renderer = ElixirRenderer(schemas)
    client, room, user = channel_events(asyncapi)

    out = [ex_header("_docs/spec/api/asyncapi.yaml")]
    out.append("defmodule WordWolf.Contract.Events do")
    out.append('  @moduledoc """')
    out.append("  Channel契約（asyncapi.yaml）のイベント名とpayload型。")
    out.append("")
    out.append("  イベント名は定数で持ち、Channelのハンドラに文字列を直書きしない。")
    out.append("  openapi.yaml と同名スキーマでも内容が一致しない場合があるため")
    out.append("  （例: `GameResult` は REST のみ `myVote` を持つ）、独立に生成している。")
    out.append('  """')
    out.append("")

    for var, pairs, doc in (
        ("client_events", client, "クライアント→サーバー（room チャンネル）"),
        ("server_room_events", room, "サーバー→部屋の全接続（room チャンネル）"),
        ("server_user_events", user, "サーバー→本人のみ（user チャンネル）"),
    ):
        out.append("  # %s" % doc)
        out.append("  @%s %s" % (var, sigil_w([e for e, _ in pairs], "  ")))
        out.append("")

    for name, body in (
        ("client_events", "@client_events"),
        ("server_room_events", "@server_room_events"),
        ("server_user_events", "@server_user_events"),
        ("server_events", "@server_room_events ++ @server_user_events"),
        ("all_events", "@client_events ++ server_events()"),
    ):
        out.append("  @spec %s() :: [String.t()]" % name)
        out.append("  def %s, do: %s" % (name, body))
        out.append("")

    out.append('  @doc "契約に定義されたイベント名かどうか。"')
    out.append("  @spec known_event?(String.t()) :: boolean()")
    out.append("  def known_event?(name) when is_binary(name), do: name in all_events()")
    out.append("")

    if schemas:
        out.append("  # --- asyncapi.yaml の components.schemas ---")
        out.append("")
        for name, schema in schemas.items():
            note = first_line(schema.get("description"))
            if note:
                out.append("  # %s" % note)
            out.extend(ex_type_lines(snake(name), renderer.render(schema, 8, name)))
            if "enum" in schema and schema.get("type") == "string":
                out.append("")
                out.append("  @spec %s_values() :: [String.t()]" % snake(name))
                out.append("  def %s_values, do: %s" % (snake(name), sigil_w(schema["enum"], "    ")))
            out.append("")

    out.append("  # --- イベントpayload ---")
    out.append("")
    mapping, _ = message_types(asyncapi, snake, "_payload")
    for event, key in client + room + user:
        message = messages[key]
        type_name, declare = mapping[event]
        if not declare:
            out.append("  # %s → %s()" % (event, type_name))
            out.append("")
            continue
        out.append("  # %s" % message.get("title", event))
        body = renderer.render(message.get("payload", {}), 8, event)
        out.extend(ex_type_lines(type_name, body))
        out.append("")

    out.append("  # --- イベント名 → payload型の対応 ---")
    out.append("  #")
    for event, _ in client + room + user:
        out.append("  #   %-26s %s()" % (event, mapping[event][0]))
    out.append("")
    out.append("end")
    return "\n".join(out).rstrip() + "\n", renderer


def build_events_ts(asyncapi):
    schemas = asyncapi["components"].get("schemas", {})
    messages = asyncapi["components"]["messages"]
    renderer = TypeScriptRenderer(schemas)
    client, room, user = channel_events(asyncapi)

    out = [ts_header("_docs/spec/api/asyncapi.yaml")]

    for name, schema in schemas.items():
        note = first_line(schema.get("description"))
        if note:
            out.append("/** %s */" % note)
        body = renderer.render(schema, 0, name)
        keyword = "interface" if body.startswith("{") else "type"
        if keyword == "interface":
            out.append("export interface %s %s" % (pascal(name), body))
        else:
            out.append("export type %s = %s;" % (pascal(name), body))
        out.append("")

    out.append("/* --- イベントpayload --- */")
    out.append("")
    mapping, renamed = message_types(asyncapi, pascal, "Payload")
    for event, key in client + room + user:
        message = messages[key]
        type_name, declare = mapping[event]
        if not declare:
            continue
        out.append("/** %s */" % message.get("title", event))
        body = renderer.render(message.get("payload", {}), 0, event)
        if body.startswith("{"):
            out.append("export interface %s %s" % (type_name, body))
        else:
            out.append("export type %s = %s;" % (type_name, body))
        out.append("")

    def name_list(const, pairs, doc):
        out.append("/** %s */" % doc)
        out.append("export const %s = [" % const)
        for event, _ in pairs:
            out.append('  "%s",' % event)
        out.append("] as const;")
        out.append("")

    name_list("CLIENT_EVENTS", client, "クライアント→サーバー")
    name_list("SERVER_ROOM_EVENTS", room, "サーバー→部屋の全接続（room:{roomId}）")
    name_list("SERVER_USER_EVENTS", user, "サーバー→本人のみ（user:{userId}）")

    def payload_map(name, pairs, doc):
        out.append("/** %s */" % doc)
        out.append("export interface %s {" % name)
        for event, _ in pairs:
            out.append('  "%s": %s;' % (event, mapping[event][0]))
        out.append("}")
        out.append("")

    payload_map("ClientEventPayloads", client, "送信イベント名 → payload")
    payload_map("RoomEventPayloads", room, "room チャンネルの受信イベント名 → payload")
    payload_map("UserEventPayloads", user, "user チャンネルの受信イベント名 → payload")

    if renamed:
        print("注意: スキーマ名と衝突したためpayload型名を変更: %s" % ", ".join(renamed))

    out.append("export type ClientEventName = keyof ClientEventPayloads;")
    out.append("export type RoomEventName = keyof RoomEventPayloads;")
    out.append("export type UserEventName = keyof UserEventPayloads;")
    out.append("")
    return "\n".join(out).rstrip() + "\n", renderer


def write(path, source):
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, "w", encoding="utf-8") as handle:
        handle.write(source)
    return os.path.relpath(path, ROOT)


def main():
    parser = argparse.ArgumentParser(description="API契約から型を生成する")
    parser.add_argument(
        "--only", choices=["all", "frontend", "backend"], default="all",
        help="生成対象（既定: all）",
    )
    args = parser.parse_args()

    for path in (OPENAPI, ASYNCAPI):
        if not os.path.exists(path):
            sys.exit("契約が見つかりません: %s" % path)

    with open(OPENAPI, encoding="utf-8") as handle:
        openapi = yaml.safe_load(handle)
    with open(ASYNCAPI, encoding="utf-8") as handle:
        asyncapi = yaml.safe_load(handle)

    renderers = []

    if args.only in ("all", "frontend"):
        source, renderer = build_events_ts(asyncapi)
        renderers.append(renderer)
        print("generated: %s" % write(os.path.join(TS_DIR, "events.ts"), source))

    if args.only in ("all", "backend"):
        source, renderer = build_types_ex(openapi)
        renderers.append(renderer)
        print("generated: %s  (%d schemas)"
              % (write(os.path.join(EX_DIR, "types.ex"), source),
                 len(openapi["components"]["schemas"])))

        source, renderer = build_events_ex(asyncapi)
        renderers.append(renderer)
        print("generated: %s  (%d events)"
              % (write(os.path.join(EX_DIR, "events.ex"), source),
                 sum(len(group) for group in channel_events(asyncapi))))

    unresolved = sorted({path for r in renderers for path in r.unresolved})
    if unresolved:
        print("\n注意: 契約に型が書かれておらず任意マップにした箇所:")
        for path in unresolved:
            print("  - %s" % path)


if __name__ == "__main__":
    main()
