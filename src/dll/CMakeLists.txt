set(TARGET_NAME smgm)

find_package(fmt REQUIRED)
find_package(spdlog 1.11 REQUIRED)
find_package(Boost 1.80.0)

file(GLOB_RECURSE TARGET_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h*)
file(GLOB_RECURSE TARGET_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx)
add_library(${TARGET_NAME} SHARED
  ${TARGET_HEADERS}
  ${TARGET_SRCS}
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
)
target_link_libraries(${TARGET_NAME}
  PUBLIC detours
  PUBLIC fmt::fmt
  PUBLIC spdlog::spdlog
  PUBLIC Boost::Boost
  PUBLIC xinput
)
add_dependencies(${TARGET_NAME} DetoursLib)
