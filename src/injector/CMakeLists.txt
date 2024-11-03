set(TARGET_NAME smgm-loader)

find_package(fmt REQUIRED)
find_package(spdlog 1.11 REQUIRED)
find_package(Boost 1.80.0)

add_executable(${TARGET_NAME}
    main.cxx
)
target_include_directories(${TARGET_NAME}
    PUBLIC ${spdlog_INCLUDE_DIR}
    PUBLIC ${fmt_INCLUDE_DIR}
)
target_compile_definitions(${TARGET_NAME}
    PUBLIC SPDLOG_FMT_EXTERNAL
)
target_link_libraries(${TARGET_NAME}
    PUBLIC fmt::fmt
    PUBLIC spdlog::spdlog
)
