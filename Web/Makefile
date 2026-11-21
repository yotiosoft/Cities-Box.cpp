# Convenience wrapper. CMake's generated files live in ./build.
BUILD_DIR ?= build
BUILD_TYPE ?= RelWithDebInfo
CMAKE_PREFIX_PATH ?= $(HOME)/.local
CMAKE_ARGS ?=

.PHONY: all configure clean run

all: configure
	cmake --build "$(BUILD_DIR)" --parallel

configure:
	cmake -S . -B "$(BUILD_DIR)" \
		-DCMAKE_BUILD_TYPE="$(BUILD_TYPE)" \
		-DCMAKE_PREFIX_PATH="$(CMAKE_PREFIX_PATH)" \
		$(CMAKE_ARGS)

run: all
	./Cities-Box_Lin

clean:
	cmake --build "$(BUILD_DIR)" --target clean
