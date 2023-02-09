cmake_minimum_required(VERSION 3.1)

project(
  DerClou
  VERSION 0.9
  LANGUAGES C)


find_package(SDL2 REQUIRED)

subdirs(src)

install(DIRECTORY gamedata/ DESTINATION share/DerClou)