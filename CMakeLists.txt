cmake_minimum_required(VERSION 3.1)

project(
  DerClou
  VERSION 0.9
  LANGUAGES C)


find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL12 REQUIRED sdl12_compat)

subdirs(src)