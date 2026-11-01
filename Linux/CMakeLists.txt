cmake_minimum_required(VERSION 3.16)

project(CitiesBoxLinux LANGUAGES CXX)

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Build type" FORCE)
endif()

find_package(Siv3D CONFIG REQUIRED)
find_program(CARGO_EXECUTABLE cargo REQUIRED)

set(CMAKE_CXX_EXTENSIONS OFF)

set(CITIES_BOX_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src")
set(CITIES_BOX_BRIDGE_DIR
	"${CMAKE_CURRENT_SOURCE_DIR}/../Windows/Cities-Box.cpp/Cities-Box.cpp")
set(CITIES_BOX_RUST_MANIFEST
	"${CITIES_BOX_SOURCE_DIR}/citiesbox-rs/Cargo.toml")

if(CMAKE_BUILD_TYPE MATCHES "^(Release|RelWithDebInfo|MinSizeRel)$")
	set(CITIES_BOX_CARGO_ARGUMENTS --release)
	set(CITIES_BOX_CARGO_PROFILE release)
else()
	set(CITIES_BOX_CARGO_ARGUMENTS)
	set(CITIES_BOX_CARGO_PROFILE debug)
endif()

set(CITIES_BOX_CARGO_TARGET_DIR "${CMAKE_CURRENT_BINARY_DIR}/cargo")
set(CITIES_BOX_RUST_LIBRARY
	"${CITIES_BOX_CARGO_TARGET_DIR}/${CITIES_BOX_CARGO_PROFILE}/libcitiesbox_rs.a")

file(GLOB_RECURSE CITIES_BOX_RUST_SOURCES CONFIGURE_DEPENDS
	"${CITIES_BOX_SOURCE_DIR}/citiesbox-rs/src/*.rs")

add_custom_command(
	OUTPUT "${CITIES_BOX_RUST_LIBRARY}"
	COMMAND
		"${CMAKE_COMMAND}" -E env
		"CARGO_TARGET_DIR=${CITIES_BOX_CARGO_TARGET_DIR}"
		"${CARGO_EXECUTABLE}" build
		--manifest-path "${CITIES_BOX_RUST_MANIFEST}"
		${CITIES_BOX_CARGO_ARGUMENTS}
	DEPENDS
		"${CITIES_BOX_RUST_MANIFEST}"
		"${CITIES_BOX_SOURCE_DIR}/citiesbox-rs/Cargo.lock"
		${CITIES_BOX_RUST_SOURCES}
	WORKING_DIRECTORY "${CITIES_BOX_SOURCE_DIR}/citiesbox-rs"
	COMMENT "Building citiesbox-rs (${CITIES_BOX_CARGO_PROFILE})"
	VERBATIM
)

add_custom_target(citiesbox_rs_build DEPENDS "${CITIES_BOX_RUST_LIBRARY}")
add_library(citiesbox_rs STATIC IMPORTED GLOBAL)
set_target_properties(citiesbox_rs PROPERTIES
	IMPORTED_LOCATION "${CITIES_BOX_RUST_LIBRARY}")
add_dependencies(citiesbox_rs citiesbox_rs_build)

set(CITIES_BOX_SOURCES
	Main.cpp
	Specific.cpp
	${CITIES_BOX_SOURCE_DIR}/AbstractSetting.cpp
	${CITIES_BOX_SOURCE_DIR}/AddonLayer.cpp
	${CITIES_BOX_SOURCE_DIR}/AddonMaker.cpp
	${CITIES_BOX_SOURCE_DIR}/AddonType.cpp
	${CITIES_BOX_SOURCE_DIR}/Button.cpp
	${CITIES_BOX_SOURCE_DIR}/CBAddon.cpp
	${CITIES_BOX_SOURCE_DIR}/CitiesBox.cpp
	${CITIES_BOX_SOURCE_DIR}/CityMap_Build.cpp
	${CITIES_BOX_SOURCE_DIR}/CityMap_Draw.cpp
	${CITIES_BOX_SOURCE_DIR}/CityMap_GetData.cpp
	${CITIES_BOX_SOURCE_DIR}/CityMap_LoadData.cpp
	${CITIES_BOX_SOURCE_DIR}/CityMap_Road.cpp
	${CITIES_BOX_SOURCE_DIR}/CityMap_SaveData.cpp
	${CITIES_BOX_SOURCE_DIR}/ConnectableObject.cpp
	${CITIES_BOX_SOURCE_DIR}/DetailsBar.cpp
	${CITIES_BOX_SOURCE_DIR}/GeneralSetting.cpp
	${CITIES_BOX_SOURCE_DIR}/Images.cpp
	${CITIES_BOX_SOURCE_DIR}/Menu.cpp
	${CITIES_BOX_SOURCE_DIR}/NormalObject.cpp
	${CITIES_BOX_SOURCE_DIR}/Object.cpp
	${CITIES_BOX_SOURCE_DIR}/PulldownMenu.cpp
	${CITIES_BOX_SOURCE_DIR}/RouteSolver.cpp
	${CITIES_BOX_SOURCE_DIR}/Sound.cpp
	${CITIES_BOX_SOURCE_DIR}/StartUp.cpp
	${CITIES_BOX_SOURCE_DIR}/SubWindow.cpp
	${CITIES_BOX_SOURCE_DIR}/Tile.cpp
	${CITIES_BOX_SOURCE_DIR}/TitleMenu.cpp
	${CITIES_BOX_SOURCE_DIR}/UnitaryTools.cpp
	${CITIES_BOX_BRIDGE_DIR}/rs-citymap.cpp
	${CITIES_BOX_BRIDGE_DIR}/rs-images.cpp
)

add_executable(Cities-Box_Lin ${CITIES_BOX_SOURCES})
add_dependencies(Cities-Box_Lin citiesbox_rs_build)

target_compile_features(Cities-Box_Lin PRIVATE cxx_std_20)
target_compile_options(Cities-Box_Lin PRIVATE
	-Wall
	-Wextra
	-Wno-unknown-pragmas
)
target_include_directories(Cities-Box_Lin PRIVATE
	"${CMAKE_CURRENT_SOURCE_DIR}"
	"${CITIES_BOX_SOURCE_DIR}"
	"${CITIES_BOX_BRIDGE_DIR}"
)
target_link_libraries(Cities-Box_Lin PRIVATE
	Siv3D::Siv3D
	citiesbox_rs
)

set_target_properties(Cities-Box_Lin PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
