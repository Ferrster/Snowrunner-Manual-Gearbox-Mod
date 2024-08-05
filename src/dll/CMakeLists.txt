set(TARGET_NAME smgm)

find_package(fmt 10.0 REQUIRED)
find_package(spdlog 1.11 REQUIRED)
find_package(Boost 1.80.0 CONFIG REQUIRED)

# find_package(Boost 1.80 COMPONENTS log log_setup REQUIRED)
add_library(${TARGET_NAME} SHARED
  game_data/game_data.h
  game_data/game_data.cxx
  game_data/data_types.h
  game_data/vehicle.cxx
  utils/logging.h
  utils/logging.cxx
  utils/format_helpers.h
  utils/format_helpers.cxx
  utils/input_reader.h
  utils/input_reader.cxx
  utils/hooks.h
  utils/hooks.cxx
  custom_functions.h
  custom_functions.cxx
  main.cxx
)
target_compile_definitions(${TARGET_NAME}
  PUBLIC BOOST_USE_WINAPI_VERSION=1536 # <= Win Vista
  PUBLIC NOMINMAX
  PRIVATE _CRT_SECURE_NO_WARNINGS
  PUBLIC SMGM_TITLE="${PROJECT_NAME}"
)

if(SMGM_NO_CONSOLE)
  target_compile_definitions(${TARGET_NAME}
    PRIVATE SMGM_NO_CONSOLE
  )
endif()

if(SMGM_USE_DETOURS)
  target_compile_definitions(${TARGET_NAME}
    PRIVATE SMGM_USE_DETOURS
  )
  target_link_libraries(${TARGET_NAME}
    PRIVATE detours
  )
  add_dependencies(${TARGET_NAME} DetoursLib)
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
  PUBLIC fmt::fmt
  PUBLIC spdlog::spdlog
  PUBLIC Boost::boost
  PUBLIC xinput
)
