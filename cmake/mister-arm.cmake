# Cross toolchain for the MiSTer (DE10-Nano) HPS: Cortex-A9, hard-float, glibc 2.31.
#
#   cmake -B build-hps -DCMAKE_TOOLCHAIN_FILE=cmake/mister-arm.cmake \
#         -DMISTER_TOOLCHAIN_DIR=<gcc-arm-10.2-2020.11-x86_64-arm-none-linux-gnueabihf> \
#         -DALSA_INCLUDE_DIR=<arm alsa>/include -DALSA_LIBRARY=<arm alsa>/lib/libasound.so \
#         -DNUKED_HEADLESS=ON -DCMAKE_BUILD_TYPE=Release

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(MISTER_TOOLCHAIN_DIR "$ENV{MISTER_TOOLCHAIN_DIR}" CACHE PATH "arm-none-linux-gnueabihf toolchain root")
if(NOT MISTER_TOOLCHAIN_DIR)
    message(FATAL_ERROR "Set MISTER_TOOLCHAIN_DIR to the arm-none-linux-gnueabihf toolchain root")
endif()
# try_compile() projects re-read this file without the cache
set(ENV{MISTER_TOOLCHAIN_DIR} "${MISTER_TOOLCHAIN_DIR}")
list(APPEND CMAKE_TRY_COMPILE_PLATFORM_VARIABLES MISTER_TOOLCHAIN_DIR)

set(CMAKE_C_COMPILER   "${MISTER_TOOLCHAIN_DIR}/bin/arm-none-linux-gnueabihf-gcc")
set(CMAKE_CXX_COMPILER "${MISTER_TOOLCHAIN_DIR}/bin/arm-none-linux-gnueabihf-g++")
set(CMAKE_SYSROOT "${MISTER_TOOLCHAIN_DIR}/arm-none-linux-gnueabihf/libc")

set(CMAKE_C_FLAGS_INIT   "-mcpu=cortex-a9 -mtune=cortex-a9 -mfpu=neon -mfloat-abi=hard")
set(CMAKE_CXX_FLAGS_INIT "-mcpu=cortex-a9 -mtune=cortex-a9 -mfpu=neon -mfloat-abi=hard")
set(CMAKE_EXE_LINKER_FLAGS_INIT "-static-libstdc++ -static-libgcc")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
