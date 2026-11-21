#!/usr/bin/env bash
set -euo pipefail

web_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd -P)"
port="${1:-6931}"

if [[ ! "$port" =~ ^[0-9]+$ ]] || (( port < 1 || port > 65535 )); then
    echo "Usage: $0 [port (1-65535)]" >&2
    exit 2
fi

for file in Siv3DApp.html Siv3DApp.js Siv3DApp.wasm Siv3DApp.data; do
    if [[ ! -f "$web_dir/$file" ]]; then
        echo "Missing $web_dir/$file; run Web/build.sh first." >&2
        exit 1
    fi
done

echo "Open http://localhost:$port/Siv3DApp.html"
echo "From another machine, replace localhost with this VM's IP address."
exec python3 -m http.server "$port" --bind 0.0.0.0 --directory "$web_dir"
