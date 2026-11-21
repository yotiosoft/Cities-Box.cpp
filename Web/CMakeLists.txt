cmake_minimum_required(VERSION 3.12)
project(CitiesBox_Web LANGUAGES CXX C)

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

find_package(Siv3D REQUIRED)

# OpenSiv3D/Web/App から作業ディレクトリ直下のリンクを参照
get_filename_component(CITIES_BOX_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/.." REALPATH)
set(CITIES_BOX_SRC "${CITIES_BOX_ROOT}/src")
set(CITIES_BOX_WEB "${CMAKE_CURRENT_SOURCE_DIR}")
set(SIV3D_WEB_APP "/home/ytani/git/opensiv3d-for-web/OpenSiv3D/Web/App")
set(CITIES_BOX_BRIDGE
    "${CITIES_BOX_ROOT}/Windows/Cities-Box.cpp/Cities-Box.cpp")
set(CITIES_BOX_RUST_LIB
    "${CITIES_BOX_SRC}/citiesbox-rs/target/wasm32-unknown-emscripten/release/libcitiesbox_rs.a")

if(NOT EXISTS "${CITIES_BOX_RUST_LIB}")
    message(FATAL_ERROR
        "Rust Web library not found: ${CITIES_BOX_RUST_LIB}")
endif()

# Linux 版 CMake のソース一覧を Web アプリに適用
add_executable(Siv3DApp
    "${CITIES_BOX_WEB}/Main.cpp"
    "${CITIES_BOX_WEB}/Specific.cpp"

    "${CITIES_BOX_SRC}/AbstractSetting.cpp"
    "${CITIES_BOX_SRC}/AddonLayer.cpp"
    "${CITIES_BOX_SRC}/AddonMaker.cpp"
    "${CITIES_BOX_SRC}/AddonType.cpp"
    "${CITIES_BOX_SRC}/Button.cpp"
    "${CITIES_BOX_SRC}/CBAddon.cpp"
    "${CITIES_BOX_SRC}/CitiesBox.cpp"
    "${CITIES_BOX_SRC}/CityMap_Build.cpp"
    "${CITIES_BOX_SRC}/CityMap_Draw.cpp"
    "${CITIES_BOX_SRC}/CityMap_GetData.cpp"
    "${CITIES_BOX_SRC}/CityMap_LoadData.cpp"
    "${CITIES_BOX_SRC}/CityMap_Road.cpp"
    "${CITIES_BOX_SRC}/CityMap_SaveData.cpp"
    "${CITIES_BOX_SRC}/ConnectableObject.cpp"
    "${CITIES_BOX_SRC}/DetailsBar.cpp"
    "${CITIES_BOX_SRC}/GeneralSetting.cpp"
    "${CITIES_BOX_SRC}/Images.cpp"
    "${CITIES_BOX_SRC}/Menu.cpp"
    "${CITIES_BOX_SRC}/NormalObject.cpp"
    "${CITIES_BOX_SRC}/Object.cpp"
    "${CITIES_BOX_SRC}/PulldownMenu.cpp"
    "${CITIES_BOX_SRC}/RouteSolver.cpp"
    "${CITIES_BOX_SRC}/Sound.cpp"
    "${CITIES_BOX_SRC}/StartUp.cpp"
    "${CITIES_BOX_SRC}/SubWindow.cpp"
    "${CITIES_BOX_SRC}/Tile.cpp"
    "${CITIES_BOX_SRC}/TitleMenu.cpp"
    "${CITIES_BOX_SRC}/UnitaryTools.cpp"

    "${CITIES_BOX_WEB}/bridge/rs-citymap.cpp"
    "${CITIES_BOX_WEB}/bridge/rs-images.cpp"
)

target_compile_features(Siv3DApp PRIVATE cxx_std_20)

target_include_directories(Siv3DApp PRIVATE
    "${CITIES_BOX_WEB}/bridge"
    "${CITIES_BOX_SRC}"
    "${CITIES_BOX_WEB}"
    "${CITIES_BOX_BRIDGE}"
)

# 元の Siv3D Web サンプルの Asyncify 設定
set(SIV3D_ASYNCIFY_ADD
    main
    "Main()"
    dynCall_v
    dynCall_vi
    "s3d::TryMain()"
    "s3d::CSystem::init()"
    "s3d::System::Update()"

    "s3d::AACDecoder::decode(*) const"
    "s3d::MP3Decoder::decode(*) const"
    "s3d::CAudioDecoder::decode(*)"
    "s3d::AudioDecoder::Decode(*)"
    "s3d::Wave::Wave(*)"
    "s3d::Audio::Audio(*)"

    "s3d::GenericDecoder::decode(*) const"
    "s3d::CImageDecoder::decode(*)"
    "s3d::Image::Image(*)"
    "s3d::Texture::Texture(*)"
    "s3d::ImageDecoder::Decode(*)"
    "s3d::ImageDecoder::GetImageInfo(*)"

    "s3d::Model::Model(*)"
    "s3d::CModel::create(*)"

    "s3d::CRenderer2D_GLES3::init()"
    "s3d::CRenderer2D_WebGPU::init()"

    "s3d::Clipboard::GetText(*)"
    "s3d::CClipboard::getText(*)"

    "s3d::SimpleHTTP::Save(*)"
    "s3d::SimpleHTTP::Load(*)"
    "s3d::SimpleHTTP::Get(*)"
    "s3d::SimpleHTTP::Post(*)"

    "s3d::VideoReader::VideoReader(*)"
    "s3d::VideoReader::open(*)"

    "s3d::Platform::Web::FetchFile(*)"
)

set(SIV3D_ASYNCIFY_IMPORTS
    siv3dRequestAnimationFrame
    siv3dGetClipboardText
    siv3dDecodeImageFromFile
    siv3dSleepUntilWaked
    invoke_v
    invoke_vi
)

function(target_add_listed_link_options target option_name)
    set(option_list ${ARGN})
    list(TRANSFORM option_list REPLACE ".+" "'\\0'")
    string(REPLACE ";" "," option_list "${option_list}")
    target_link_options(${target} PRIVATE
        -s${option_name}=[${option_list}])
endfunction()

target_add_listed_link_options(
    Siv3DApp ASYNCIFY_IMPORTS ${SIV3D_ASYNCIFY_IMPORTS})
target_add_listed_link_options(
    Siv3DApp ASYNCIFY_ADD ${SIV3D_ASYNCIFY_ADD})

target_link_options(Siv3DApp PRIVATE
    --no-export-dynamic
    -sASYNCIFY=1
    -sUSE_WEBGPU=1
    --emrun
    -sMODULARIZE=1
    -sEXPORT_NAME=Runtime
    "--shell-file=${SIV3D_WEB_APP}/Templates/Embeddable/web-player.html"
    "--post-js=${SIV3D_WEB_APP}/Templates/Embeddable/web-player.js"

    # Siv3D Web サンプルのエンジン素材
    "--preload-file=${CMAKE_CURRENT_SOURCE_DIR}/resources/engine@/resources/engine"

    # Cities Box の素材。実行時には ./data などで参照できる
    "--preload-file=${CITIES_BOX_WEB}/data@/data"
    "--preload-file=${CITIES_BOX_WEB}/addons@/addons"
    "--preload-file=${CITIES_BOX_WEB}/fonts@/fonts"
    "--preload-file=${CITIES_BOX_WEB}/sound@/sound"
)

target_link_libraries(Siv3DApp PRIVATE
    -Wl,--whole-archive
    Siv3D::Siv3DWebGPU
    -Wl,--no-whole-archive
    Siv3D::Siv3D
    Siv3D::Siv3DScript
    "${CITIES_BOX_RUST_LIB}"
)

set_target_properties(Siv3DApp PROPERTIES
    SUFFIX ".html"
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}"
)
