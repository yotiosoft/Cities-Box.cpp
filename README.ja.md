# Cities Box.cpp

日本語 | [English](README.md)

## 概要

複数のプラットフォームで動作する無料の街づくりゲームです。現在、対応言語は日本語のみです。

## 関連リポジトリ

2019 年以前の Hot Soup Processor（HSP）版: https://github.com/yotiosoft/Cities-Box

## 開発環境

- Windows: Windows 10 Home 64bit、Windows SDK 10.0、Visual Studio 2022、Intel x64（Core i5-6500）
- macOS: macOS 12.2.1 Monterey、Xcode 13.3、Intel x64（Apple M1/M2 でも Rosetta 2 経由で動作）
- 共通: OpenSiv3D 0.6.16、C++、Rust

## ビルド方法

**Windows・macOS・Linux のいずれでも、ビルドには OpenSiv3D 0.6.16 の導入が必須です。** 対象 OS 向けに事前に導入してください。このリポジトリには OpenSiv3D の SDK とライブラリは含まれていません。

### Windows

1. Visual Studio の「C++ によるデスクトップ開発」ワークロード、Windows SDK、プロジェクトで指定されている MSVC `v145` ツールセットを導入します。Rust と Cargo、および `x86_64-pc-windows-msvc` ターゲットも導入します。
2. 環境変数 `SIV3D_0_6_16` に OpenSiv3D 0.6.16 のルートディレクトリ（`include` と `lib/Windows` があるディレクトリ）を指定します。
3. Visual Studio で `Windows/Cities-Box.cpp/Cities-Box.cpp.slnx` を開き、`x64` と `Debug` または `Release` を選んでソリューションをビルドします。ビルド前に Cargo で `src/citiesbox-rs` がビルドされ、生成した実行ファイルは `Windows/Cities-Box.cpp/Cities-Box.cpp/App` にコピーされます。

### macOS

1. Xcode、Rust、Cargo、`cxxbridge-cmd`（`cargo install cxxbridge-cmd`）を導入します。Intel Mac では `rustup target add x86_64-apple-darwin`、Apple Silicon では `rustup target add aarch64-apple-darwin` を実行します。
2. Xcode プロジェクトが参照するパスに OpenSiv3D 0.6.16 のヘッダーとライブラリを配置します。リポジトリのルートにある `include` と `lib/macOS` が参照先です。
3. Xcode で `Mac/Cities-Box/Cities-Box.xcodeproj` を開き、`Cities-Box` スキームと使用する Mac を選んでビルドします。ビルドフェーズで Rust ライブラリと CXX ブリッジファイルが生成されます。

Xcode プロジェクトにはソースへの絶対パス（`/Users/ytani/git/CitiesBox.cpp/src/citiesbox-rs`）と `x86_64-apple-darwin` 向け Rust ライブラリへの参照が含まれています。ビルド前に、チェックアウト先と使用するアーキテクチャに合わせてプロジェクトの参照先を修正してください。Apple Silicon では特に注意が必要です。

### Linux

1. C++20 対応コンパイラー、CMake 3.16 以降、Rust、Cargo を導入します。OpenSiv3D 0.6.16 とその開発用依存ライブラリを Linux 向けに導入し、CMake から `Siv3D` パッケージを検出できる状態にします。
2. リポジトリのルートで次を実行します。CMake が Siv3D を検出できない場合は、`CMAKE_PREFIX_PATH` に OpenSiv3D のインストール先プレフィックスを指定してください。

   ```sh
   cmake -S Linux -B build/linux -DCMAKE_BUILD_TYPE=Release
   cmake --build build/linux -j
   ```

Linux 向け CMake ターゲットは `src/citiesbox-rs` を自動的にビルドしてリンクします。リソースを読み込めるよう、実行時は `Linux` ディレクトリを作業ディレクトリにしてください。リポジトリのルートから `Linux/run.sh` を使って起動できます。
