cmake_minimum_required(VERSION 3.23)

project(SMGM VERSION 0.2 LANGUAGES CXX)

if(NOT MSVC)
  message(FATAL_ERROR "This project supports only Visual C++ compilers!")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SMGM_NO_CONSOLE OFF CACHE BOOL "Do not use console window for log display")
set(SMGM_BUILD_DLL ON CACHE BOOL "Build mod's DLL")
set(SMGM_BUILD_INJECTOR ON CACHE BOOL "Build simple mod injector")

add_subdirectory(src)
