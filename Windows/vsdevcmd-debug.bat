cd ""%2"\..\..\..\src\citiesbox-rs"
set RUSTFLAGS=-C target-feature=+crt-static
set CXXFLAGS=/MTd /D_DEBUG /Zi /Od /D_ITERATOR_DEBUG_LEVEL=2
set CFLAGS=/MTd /D_DEBUG /Zi /Od /D_ITERATOR_DEBUG_LEVEL=2
cargo build --target x86_64-pc-windows-msvc --target-dir "%2"
cxxbridge src/lib.rs --header > ""%2"\citiesbox-rs.h"
cxxbridge src/images.rs --header > ""%2"\rs-images.h"
cxxbridge src/citymap.rs --header > ""%2"\rs-citymap.h"
cxxbridge src/lib.rs > ""%2"\citiesbox-rs.cpp"
cxxbridge src/images.rs > ""%2"\rs-images.cpp"
cxxbridge src/citymap.rs > ""%2"\rs-citymap.cpp"
cxxbridge --header > ""%2"\cxx.h"

cd "..\..\"
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
msbuild ""%1".vcxproj"
cd ""%2/App"
"%3(debug).exe"
