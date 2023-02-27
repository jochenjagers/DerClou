set(CMAKE_SYSTEM_NAME Windows)
set(TOOLCHAIN_PREFIX x86_64-w64-mingw32)

message(STATUS "Using toolchain file ${CMAKE_TOOLCHAIN_FILE}")

# cross compilers to use for C, C++
set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}-g++)

cmake_policy(SET CMP0135 NEW)

include(FetchContent)
FetchContent_Declare(
  SDL2
  URL      https://github.com/libsdl-org/SDL/releases/download/release-2.26.3/SDL2-devel-2.26.3-mingw.tar.gz
  URL_HASH MD5=ac18b288bcd965a1d415508eddc4e01d
  )
FetchContent_MakeAvailable(SDL2)
set(SDL2_ROOT ${sdl2_SOURCE_DIR}/cmake)

set(CMAKE_INSTALL_PREFIX ".")
set(INSTALL_PATH_DATA ".")
set(INSTALL_PATH_BIN ".")

add_link_options("$<$<CONFIG:DEBUG>:-mconsole>")
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/_deps/sdl2-src/x86_64-w64-mingw32/bin/SDL2.dll DESTINATION ${INSTALL_PATH_BIN})

