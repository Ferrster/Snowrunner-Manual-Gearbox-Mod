set(TARGET_NAME snowrunner-manual-gearbox-mod)

find_package(fmt REQUIRED)
find_package(spdlog 1.11 REQUIRED)

# find_package(Boost 1.80 COMPONENTS log log_setup REQUIRED)
add_library(${TARGET_NAME} SHARED
  game_data/game_data.h
  game_data/game_data.cxx
  game_data/data_types.h
  utils/logging.h
  utils/logging.cxx
  utils/format_helpers.h
  utils/format_helpers.cxx
  custom_functions.h
  custom_functions.cxx
  main.cxx
)
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
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(${TARGET_NAME}
  PUBLIC detours
  PUBLIC fmt::fmt
  PUBLIC spdlog::spdlog
  PUBLIC xinput
)
add_dependencies(${TARGET_NAME} DetoursLib)
