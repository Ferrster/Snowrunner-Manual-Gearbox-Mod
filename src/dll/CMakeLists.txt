set(TARGET_NAME smgm)

find_package(fmt REQUIRED)
find_package(spdlog 1.11 REQUIRED)
find_package(Boost 1.80.0)

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
  PUBLIC BOOST_USE_WINAPI_VERSION=1536 # <= Win Vista
  PUBLIC NOMINMAX
  PRIVATE _CRT_SECURE_NO_WARNINGS
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
  PRIVATE ${BOOST_INCLUDE_DIR}
  PRIVATE ${SPDLOG_INCLUDE_DIR}
  PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
  PUBLIC ${imgui_SOURCE_DIR}
)
target_link_libraries(${TARGET_NAME}
  PUBLIC detours
  PUBLIC fmt::fmt
  PUBLIC spdlog::spdlog
  PUBLIC Boost::Boost
  PUBLIC xinput
  PRIVATE $<TARGET_OBJECTS:ImGui>
)
add_dependencies(${TARGET_NAME} DetoursLib)
