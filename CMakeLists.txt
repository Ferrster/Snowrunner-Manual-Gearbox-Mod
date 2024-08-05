cmake_minimum_required(VERSION 3.14)

project(SMGM VERSION 0.1 LANGUAGES CXX)

if(NOT MSVC)
  message(FATAL_ERROR "This project supports only Visual C++ compilers!")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SMGM_NO_CONSOLE OFF CACHE BOOL "Do not use console window for log display")
set(SMGM_USE_DETOURS ON CACHE BOOL "Use detours for hooking")

if(SMGM_USE_DETOURS)
  set(SMGM_PATH_TO_VCVARS64 "G:\\Programs\\Visual Studio 19\\VC\\Auxiliary\\Build\\vcvars64.bat" CACHE PATH "Path to vcvars64.bat. Needed to build detours library")
  include(ExternalProject)
  ExternalProject_Add(DetoursLib
    GIT_REPOSITORY https://github.com/microsoft/Detours.git
    GIT_TAG origin/main
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
  )
  ExternalProject_Get_Property(DetoursLib SOURCE_DIR)
  ExternalProject_Add_Step(DetoursLib _build
    COMMAND ${PROJECT_SOURCE_DIR}/misc/build_detours.bat ${SMGM_PATH_TO_VCVARS64} ${SOURCE_DIR}
    ALWAYS ON
  )
  link_directories(${SOURCE_DIR}/lib.X64)
  include_directories(${SOURCE_DIR}/include)
endif()

add_subdirectory(src)
