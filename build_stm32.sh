#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_DIR="$SCRIPT_DIR"
BUILD_DIR="$PROJECT_DIR/build"
BUILD_TYPE="${BUILD_TYPE:-Debug}"

TOOLCHAIN_PATH="${TOOLCHAIN_PATH:-/Volumes/disk1t/gcc/ARM}"
TOOLCHAIN_BIN_PATH="${TOOLCHAIN_BIN_PATH:-$TOOLCHAIN_PATH/bin}"
STM32_HAL_PATH="${STM32_HAL_PATH:-/Volumes/disk1t/stm32/STM32WB55}"

JOBS="${JOBS:-$(sysctl -n hw.ncpu 2>/dev/null || echo 4)}"

usage() {
  echo "Usage: $0 [command] [options]"
  echo ""
  echo "Commands:"
  echo "  config    Run CMake configuration only"
  echo "  build     Build the project (default)"
  echo "  clean     Clean build directory"
  echo "  rebuild   Clean and rebuild"
  echo "  help      Show this help message"
  echo ""
  echo "Options:"
  echo "  BUILD_TYPE=Debug|Release   Build type (default: Debug)"
  echo "  TOOLCHAIN_PATH=<path>      ARM toolchain root (default: /Volumes/disk1t/gcc/ARM)"
  echo "  STM32_HAL_PATH=<path>      STM32WB55 HAL path (default: /Volumes/disk1t/stm32/STM32WB55)"
  echo "  JOBS=<n>                   Parallel build jobs (default: CPU count)"
  echo ""
  echo "Examples:"
  echo "  $0                        # Debug build"
  echo "  $0 rebuild                # Clean and rebuild"
  echo "  BUILD_TYPE=Release $0     # Release build"
}

check_toolchain() {
  local gcc="$TOOLCHAIN_BIN_PATH/arm-none-eabi-gcc"
  local ar="$TOOLCHAIN_BIN_PATH/arm-none-eabi-ar"
  if [ ! -f "$gcc" ]; then
    echo "ERROR: arm-none-eabi-gcc not found at $gcc"
    echo "Set TOOLCHAIN_PATH or TOOLCHAIN_BIN_PATH to the correct location."
    exit 1
  fi
  if [ ! -f "$ar" ]; then
    echo "ERROR: arm-none-eabi-ar not found at $ar"
    echo "Set TOOLCHAIN_PATH to a toolchain that includes ar."
    exit 1
  fi
  echo "Toolchain: $gcc"
  $gcc --version | head -1
}

check_hal() {
  if [ ! -d "$STM32_HAL_PATH/Drivers" ]; then
    echo "ERROR: STM32WB55 HAL not found at $STM32_HAL_PATH"
    echo "Set STM32_HAL_PATH to the correct location."
    exit 1
  fi
  echo "STM32 HAL: $STM32_HAL_PATH"
}

init_submodules() {
  echo "Checking git submodules..."
  local need_init=0
  if [ ! -f "$PROJECT_DIR/src/libs/lvgl/lvgl.h" ]; then
    need_init=1
  fi
  if [ ! -f "$PROJECT_DIR/src/libs/littlefs/lfs_util.h" ]; then
    need_init=1
  fi
  if [ "$need_init" = "1" ]; then
    echo "Initializing git submodules (lvgl, littlefs)..."
    cd "$PROJECT_DIR"
    git submodule update --init src/libs/littlefs src/libs/lvgl
  else
    echo "Submodules already initialized."
  fi
}

run_cmake() {
  echo ""
  echo "=== CMake Configuration ==="
  echo "  Build type:  $BUILD_TYPE"
  echo "  Toolchain:   $TOOLCHAIN_PATH"
  echo "  STM32 HAL:   $STM32_HAL_PATH"
  echo "  Jobs:        $JOBS"
  echo ""

  mkdir -p "$BUILD_DIR"
  cd "$BUILD_DIR"

  cmake -G "Unix Makefiles" \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -DCMAKE_TOOLCHAIN_FILE="$PROJECT_DIR/cmake-nRF5x/arm-gcc-toolchain.cmake" \
    -DARM_NONE_EABI_TOOLCHAIN_PATH="$TOOLCHAIN_PATH" \
    -DARM_NONE_EABI_TOOLCHAIN_BIN_PATH="$TOOLCHAIN_BIN_PATH" \
    -DSTM32WB55_HAL_PATH="$STM32_HAL_PATH" \
    "$PROJECT_DIR"
}

run_build() {
  echo ""
  echo "=== Building ==="
  cd "$BUILD_DIR"
  cmake --build . --config "$BUILD_TYPE" -- -j"$JOBS"
}

cmd="${1:-build}"
case "$cmd" in
  config)
    check_toolchain
    check_hal
    init_submodules
    run_cmake
    ;;
  build)
    check_toolchain
    check_hal
    init_submodules
    if [ ! -f "$BUILD_DIR/Makefile" ]; then
      run_cmake
    fi
    run_build
    ;;
  clean)
    echo "Cleaning build directory..."
    rm -rf "$BUILD_DIR"
    echo "Done."
    ;;
  rebuild)
    echo "Cleaning and rebuilding..."
    rm -rf "$BUILD_DIR"
    check_toolchain
    check_hal
    init_submodules
    run_cmake
    run_build
    ;;
  help|--help|-h)
    usage
    ;;
  *)
    echo "Unknown command: $cmd"
    usage
    exit 1
    ;;
esac