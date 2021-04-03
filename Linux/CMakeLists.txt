cmake_minimum_required (VERSION 3.0)

find_package(PkgConfig)

project(OpenSiv3D_Linux CXX)
enable_language(C)

set(CMAKE_CXX_FLAGS "-std=c++17 -Wall -Wextra -Wno-unknown-pragmas -fPIC -msse4.1")
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0 -pg -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG -march=x86-64")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g3 -Og -pg")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG -march=x86-64")

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	add_compile_options ("-fcolor-diagnostics")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	add_compile_options ("-fdiagnostics-color=always")
endif()

#set(CMAKE_BUILD_TYPE Release)

pkg_check_modules(OPENCV4 REQUIRED opencv4)

include_directories(
	"/usr/include"

	"~/OpenSiv3D/Siv3D/include"
	"~/OpenSiv3D/Siv3D/include/ThirdParty"
	"./"
	"../src"
)

set(SOURCE_FILES
	"./Main.cpp"
	"./Specific.cpp"
	"../src/AbstractSetting.cpp"
	"../src/Addon.cpp"
	"../src/AddonLayer.cpp"
	"../src/AddonMaker.cpp"
	"../src/AddonType.cpp"
	"../src/Button.cpp"
	"../src/CitiesBox.cpp"
	"../src/CityMap.cpp"
	"../src/DetailsBar.cpp"
	"../src/GeneralSetting.cpp"
	"../src/Images.cpp"
	"../src/Menu.cpp"
	"../src/Object.cpp"
	"../src/OtherFunctions.cpp"
	"../src/PulldownMenu.cpp"
	"../src/RouteSolver.cpp"
	"../src/Sound.cpp"
	"../src/StartUp.cpp"
	"../src/SubWindow.cpp"
	"../src/Tile.cpp"
	"../src/TitleMenu.cpp"
)

add_executable(Cities-Box_Lin ${SOURCE_FILES})

target_link_libraries(Cities-Box_Lin
	/home/ytani0323/OpenSiv3D/Linux/Build/libSiv3D.a
	${OPENCV4_LIBRARIES}
	-lOpenGL
	-lGLEW
	-lX11
	-lpthread
	-ldl
	-ludev
	-lfreetype
	-lharfbuzz
	-lglib-2.0
	-lgobject-2.0
	-lgio-2.0
	-lpng
	-lturbojpeg
	-lgif
	-lwebp
	-lopenal
	-logg
	-lvorbis
	-lvorbisenc
	-lvorbisfile
	-lboost_filesystem
	-lavformat
	-lavcodec
	-lavutil
	-lswresample
	-lz
)

