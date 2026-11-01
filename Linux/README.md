# Linux build

The Linux target uses OpenSiv3D v0.6.16 and the Rust toolchain. Build files are
generated out of source in `Linux/build`; the old absolute paths in generated
CMake files are not used.

## Requirements

- Ubuntu 22.04 (officially supported by OpenSiv3D v0.6.16) or a compatible
  Ubuntu system with an x86-64 CPU and an OpenGL 4.1-capable GPU
- GCC, CMake, Ninja, pkg-config, and Rust/Cargo
- OpenSiv3D v0.6.16 and its development packages

Install the packages used by OpenSiv3D's official Ubuntu build:

```sh
sudo apt update
sudo apt install -y \
  build-essential cmake ninja-build pkg-config git curl \
  libasound2-dev libavcodec-dev libavformat-dev libavutil-dev libboost-dev \
  libcurl4-openssl-dev libgtk-3-dev libgif-dev libglu1-mesa-dev \
  libharfbuzz-dev libmpg123-dev libopencv-dev libopus-dev libopusfile-dev \
  libsoundtouch-dev libswresample-dev libtiff-dev libturbojpeg0-dev \
  libvorbis-dev libwebp-dev libxft-dev uuid-dev xorg-dev
```

Install Rust if `cargo --version` fails:

```sh
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
```

## Build and install OpenSiv3D

The commands below install OpenSiv3D under `~/.local`, so installing the
library itself does not require root privileges:

```sh
git clone --depth 1 --branch v0.6.16 \
  https://github.com/Siv3D/OpenSiv3D.git ~/OpenSiv3D
cmake -S ~/OpenSiv3D/Linux -B ~/OpenSiv3D/Linux/build \
  -GNinja -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build ~/OpenSiv3D/Linux/build --parallel
cmake --install ~/OpenSiv3D/Linux/build --prefix ~/.local
```

## Build Cities Box

From the repository root:

```sh
make -C Linux
```

The executable is generated at `Linux/Cities-Box_Lin`, next to the runtime resources. Run it with:

```sh
Linux/run.sh
```

If OpenSiv3D is installed under another prefix, pass it explicitly:

```sh
make -C Linux CMAKE_PREFIX_PATH=/path/to/prefix
```

For a debug build, use `make -C Linux BUILD_TYPE=Debug`.
