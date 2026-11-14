#!/bin/sh
# API ドキュメント HTML を再生成する。
#   openapi.yaml  -> index.html    （Scalar / HTTP API）
#   asyncapi.yaml -> realtime.html （AsyncAPI React Component / Phoenix Channels）
# 依存: python3 + PyYAML。閲覧時はビューア本体を CDN から読み込むためネット接続が必要。
set -eu
cd "$(dirname "$0")"

python3 - << 'EOF'
import json
import yaml


def load_spec_json(path):
    spec_json = json.dumps(yaml.safe_load(open(path)), ensure_ascii=False)
    if '</script' in spec_json.lower():
        raise SystemExit(f'{path} に </script> を含めないでください（HTML埋め込みが壊れます）')
    return spec_json


openapi_json = load_spec_json('openapi.yaml')
asyncapi_json = load_spec_json('asyncapi.yaml')

open('index.html', 'w').write(f'''<!DOCTYPE html>
<html lang="ja">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>ワードウルフ REST API</title>
</head>
<body>
<div id="app"></div>
<script src="https://cdn.jsdelivr.net/npm/@scalar/api-reference"></script>
<script>
Scalar.createApiReference('#app', {{ content: {openapi_json} }});
</script>
</body>
</html>
''')

open('realtime.html', 'w').write(f'''<!DOCTYPE html>
<html lang="ja">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>ワードウルフ Realtime API</title>
<link rel="stylesheet" href="https://unpkg.com/@asyncapi/react-component@latest/styles/default.min.css">
<style>body {{ margin: 0; }}</style>
</head>
<body>
<div id="asyncapi"></div>
<script src="https://unpkg.com/@asyncapi/react-component@latest/browser/standalone/index.js"></script>
<script>
AsyncApiStandalone.render({{
  schema: {asyncapi_json},
  config: {{ show: {{ sidebar: true }} }},
}}, document.getElementById('asyncapi'));
</script>
</body>
</html>
''')
EOF

echo "generated: $(pwd)/index.html, $(pwd)/realtime.html"
