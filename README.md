#  Cities Box.cpp

[日本語](README.ja.md) | English
## Summary
A free city-building game that works on multiple platforms.  
At now, only Japanese language is supported.

## Associated repository
Hot Soup Processor (HSP) version (before 2019):  
https://github.com/yotiosoft/Cities-Box

## Development Environment
- for Windows
	- OS : Windows 10 Home 64bit
	- Windows SDK : 10.0
	- C++ compiler：C++ 17
	- IDE : Visual Studio 2022 (v143)
	- Library：OpenSiv3D v0.6.16
	- CPU : Intel x64 cpu (Core i5-6500)
- for macOS
	- OS : macOS 12.2.1 Monterey
	- C++ compiler：C++ 17
	- IDE : Xcode 13.3
	- Library：OpenSiv3D v0.6.16
	- CPU : intel x64 cpu (also Apple M1/M2 can run on Rosetta2.)
	

## Build

**OpenSiv3D 0.6.16 is required to build this project on Windows, macOS, and Linux.** Install it for the target platform before following the steps below. The repository does not include the OpenSiv3D SDK or its libraries.

### Windows

1. Install Visual Studio with the Desktop development with C++ workload, the Windows SDK, and the MSVC `v145` platform toolset used by the project. Install Rust and Cargo with the `x86_64-pc-windows-msvc` target. Set the `SIV3D_0_6_16` environment variable to the OpenSiv3D 0.6.16 root directory (the directory containing `include` and `lib/Windows`).
2. Open `Windows/Cities-Box.cpp/Cities-Box.cpp.slnx` in Visual Studio.
3. Select `x64` and `Debug` or `Release`, then build the solution. The project builds `src/citiesbox-rs` with Cargo before compiling C++ and copies the executable into `Windows/Cities-Box.cpp/Cities-Box.cpp/App` after the build.

### macOS

1. Install Xcode, Rust and Cargo, and `cxxbridge-cmd` (`cargo install cxxbridge-cmd`). Install the Rust target for your Mac: `rustup target add x86_64-apple-darwin` on Intel or `rustup target add aarch64-apple-darwin` on Apple Silicon.
2. Provide the OpenSiv3D 0.6.16 headers and libraries at the paths referenced by the Xcode project (`include` and `lib/macOS` at the repository root).
3. Open `Mac/Cities-Box/Cities-Box.xcodeproj` in Xcode, select the `Cities-Box` scheme and your Mac as the destination, then build. Its build phase compiles the Rust library and generates the CXX bridge files.

The Xcode project currently contains an absolute source path (`/Users/ytani/git/CitiesBox.cpp/src/citiesbox-rs`) and links the `x86_64-apple-darwin` Rust library. Update these project references for your checkout and architecture before building, especially on Apple Silicon.

### Linux

1. Install a C++20 compiler, CMake 3.16 or newer, Rust and Cargo. Install OpenSiv3D 0.6.16 for Linux with its development dependencies so that CMake can find the `Siv3D` package.
2. From the repository root, configure and build. If CMake cannot find Siv3D, set `CMAKE_PREFIX_PATH` to the OpenSiv3D installation prefix:

   ```sh
   cmake -S Linux -B build/linux -DCMAKE_BUILD_TYPE=Release
   cmake --build build/linux -j
   ```

The Linux CMake target builds and links `src/citiesbox-rs` automatically. Run the game from the `Linux` directory so that its resources are available, for example with `Linux/run.sh` from the repository root.
