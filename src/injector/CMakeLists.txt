set(TARGET_NAME smgm-loader)

find_package(fmt REQUIRED)

add_executable(${TARGET_NAME}
  main.cxx
)
target_link_libraries(${TARGET_NAME}
  PUBLIC fmt::fmt
)
