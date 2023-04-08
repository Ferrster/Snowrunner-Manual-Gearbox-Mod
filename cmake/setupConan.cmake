# Download automatically, you can also just copy the conan.cmake file
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
   message(STATUS "NO conan.cmake in ${CMAKE_BINARY_DIR}. Copying...")
   file(COPY "${CMAKE_SOURCE_DIR}/cmake/conan.cmake" DESTINATION "${CMAKE_BINARY_DIR}")
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

conan_cmake_configure(REQUIRES
   fmt/9.1.0
   boost/1.80.0
   spdlog/1.11.0
   GENERATORS cmake_find_package)

conan_cmake_autodetect(settings)

conan_cmake_install(PATH_OR_REFERENCE .
   BUILD missing
   REMOTE conancenter
   SETTINGS ${settings})
