set(TARGET_NAME smgm)

find_package(fmt REQUIRED)
find_package(Boost CONFIG REQUIRED)

set(SMGM_USE_DETOURS ON CACHE BOOL "Use detours for hooking")

if(SMGM_USE_DETOURS)
  include(FetchContent)
  FetchContent_Declare(detours
    GIT_REPOSITORY https://github.com/microsoft/Detours.git
    GIT_TAG origin/main
    PATCH_COMMAND ${CMAKE_COMMAND}
    -DSMGM_PROJECT_DIR=${PROJECT_SOURCE_DIR}
    -DDETOURS_SOURCE_DIR=<SOURCE_DIR>
    -DDETOURS_SOURCE_CMAKELISTS_PATH=${PROJECT_SOURCE_DIR}/cmake/detours_cmakelists.txt
    -P ${PROJECT_SOURCE_DIR}/cmake/detours_add_cmake.cmake

    OVERRIDE_FIND_PACKAGE
  )
  FetchContent_MakeAvailable(detours)
endif()

# Check for presence of at least one hooking library (right now only Detours lib is available)
if(NOT SMGM_USE_DETOURS)
  message(FATAL_ERROR "At least one hooking library is required to build this project")
endif()

set(SMGM_USE_IMGUI ON CACHE BOOL "Use ImGui for in-game configuration of the mod")

if(SMGM_USE_IMGUI)
  include(FetchContent)
  FetchContent_Declare(ImGui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.89.8
  )
  FetchContent_MakeAvailable(ImGui)
  file(GLOB IMGUI_HEADERS ${imgui_SOURCE_DIR}/*.h)
  file(GLOB IMGUI_SRCS ${imgui_SOURCE_DIR}/*.cpp)
  file(GLOB_RECURSE IMGUI_MISC_HEADERS ${imgui_SOURCE_DIR}/misc/cpp/*.h)
  file(GLOB_RECURSE IMGUI_MISC_SRCS ${imgui_SOURCE_DIR}/misc/cpp/*.cpp)
  add_library(ImGui OBJECT
    ${IMGUI_HEADERS}
    ${IMGUI_SRCS}
    ${IMGUI_MISC_HEADERS}
    ${IMGUI_MISC_SRCS}
    ${imgui_SOURCE_DIR}/backends/imgui_impl_win32.h
    ${imgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_dx11.h
    ${imgui_SOURCE_DIR}/backends/imgui_impl_dx11.cpp
  )
  target_include_directories(ImGui
    PUBLIC ${imgui_SOURCE_DIR}
  )
endif()

file(GLOB_RECURSE TARGET_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h*)
file(GLOB_RECURSE TARGET_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx)
add_library(${TARGET_NAME} SHARED
  ${TARGET_HEADERS}
  ${TARGET_SRCS}
  $<TARGET_OBJECTS:ImGui>
)
set_target_properties(${TARGET_NAME} PROPERTIES LINKER_LANGUAGE CXX)
target_compile_definitions(${TARGET_NAME}
  PUBLIC ${Boost_DEFINITIONS}
  PUBLIC BOOST_USE_WINAPI_VERSION=1536 # <= Win Vista
  PUBLIC NOMINMAX
  PRIVATE _CRT_SECURE_NO_WARNINGS
  PRIVATE _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
  PUBLIC DIRECTINPUT_VERSION=0x0800
  PUBLIC SMGM_TITLE="${PROJECT_NAME}"
)

if(SMGM_NO_CONSOLE)
  target_compile_definitions(${TARGET_NAME}
    PRIVATE SMGM_NO_CONSOLE
  )
endif()

target_link_directories(${TARGET_NAME}
  PRIVATE ${PROJECT_SOURCE_DIR}/lib
)
target_include_directories(${TARGET_NAME}
  PUBLIC ${Boost_INCLUDE_DIR}
  PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
  PUBLIC ${imgui_SOURCE_DIR}
)

target_link_libraries(${TARGET_NAME}
  PUBLIC fmt::fmt
  PUBLIC dinput8
  PUBLIC boost::boost
  PUBLIC dxguid
  PUBLIC xinput
  PRIVATE $<TARGET_OBJECTS:ImGui>
)

if(SMGM_USE_DETOURS)
  target_compile_definitions(${TARGET_NAME}
    PRIVATE SMGM_USE_DETOURS
  )
  target_link_libraries(${TARGET_NAME}
    PRIVATE detours
  )
endif()
