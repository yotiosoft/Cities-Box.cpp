#!/usr/bin/env bash
set -euo pipefail

web_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd -P)"
app_dir="$(dirname "$web_dir")"
sdk_root="${OPEN_SIV3D_WEB_ROOT:-$(dirname "$app_dir")/opensiv3d-for-web}"
emsdk_dir="$sdk_root/emsdk"
rust_dir="$app_dir/src/citiesbox-rs"
build_dir="$web_dir/build"
siv3d_dir="$emsdk_dir/upstream/emscripten/cache/sysroot/lib/cmake/Siv3D"

if [[ ! -f "$emsdk_dir/emsdk_env.sh" ]]; then
    echo "Emscripten SDK not found: $emsdk_dir" >&2
    echo "Set OPEN_SIV3D_WEB_ROOT to the opensiv3d-for-web directory." >&2
    exit 1
fi

# SSH non-login shells do not always put Cargo on PATH.
if [[ -f "$HOME/.cargo/env" ]]; then
    source "$HOME/.cargo/env"
fi

# The installed Siv3D package and the app must use the same Emscripten SDK.
EMSDK_QUIET=1 source "$emsdk_dir/emsdk_env.sh" >/dev/null

if [[ ! -f "$siv3d_dir/Siv3DConfig.cmake" ]]; then
    echo "Siv3D Web package not installed: $siv3d_dir" >&2
    exit 1
fi

export CC=emcc CXX=em++ AR=emar
cargo +1.85.1 build --locked --manifest-path "$rust_dir/Cargo.toml" \
    --release --target wasm32-unknown-emscripten

emcmake cmake -S "$web_dir" -B "$build_dir" \
    -G 'Unix Makefiles' \
    -DCMAKE_BUILD_TYPE=Release \
    -DSiv3D_DIR="$siv3d_dir" \
    -DSIV3D_WEB_APP="$sdk_root/OpenSiv3D/Web/App"
cmake --build "$build_dir" --parallel "${BUILD_JOBS:-4}"

echo "Built $web_dir/Siv3DApp.html"
