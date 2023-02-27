cmake_minimum_required(VERSION 3.12)

project(
  DerClou
  VERSION 0.9
  LANGUAGES C)

find_package(SDL2 REQUIRED CONFIG)
subdirs(src)

if(NOT DEFINED INSTALL_PATH_DATA)
  set(INSTALL_PATH_DATA share/DerClou)
endif()
message(STATUS "Install path data is ${INSTALL_PATH_DATA}")
install(DIRECTORY gamedata/ DESTINATION ${INSTALL_PATH_DATA})
