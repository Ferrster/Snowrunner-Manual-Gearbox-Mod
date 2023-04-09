set(TARGET_NAME smgm-loader)

find_package(fmt REQUIRED)
find_package(spdlog 1.11 REQUIRED)
find_package(Boost 1.80.0)

add_executable(${TARGET_NAME}
  main.cxx
)
target_include_directories(${TARGET_NAME}
  PUBLIC ${SPDLOG_INCLUDE_DIR}
)
target_link_libraries(${TARGET_NAME}
  PUBLIC fmt::fmt
  PUBLIC spdlog::spdlog
)
