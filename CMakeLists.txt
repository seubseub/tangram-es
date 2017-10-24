cmake_minimum_required(VERSION 3.0)

project(tangram)

# Options
option(TANGRAM_USE_SYSTEM_FONT_LIBS "Use system libraries Freetype, ICU and Harfbuzz via pkgconfig" OFF)
option(TANGRAM_USE_SYSTEM_GLFW_LIBS "Use system libraries for GLFW3 via pkgconfig" OFF)

if(NOT ${CMAKE_BUILD_TYPE} STREQUAL "")
  message(STATUS "Build type configuration " ${CMAKE_BUILD_TYPE})
else()
  set(CMAKE_BUILD_TYPE Release)
endif()

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/core/deps/harfbuzz-icu-freetype/harfbuzz/README")
  message(SEND_ERROR "Missing submodules - Please run:\n 'git submodule update --init'")
  return()
endif()

set(TANGRAM_APPLICATION ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

include(${PROJECT_SOURCE_DIR}/toolchains/utils.cmake)

# platform lookup
set(SUPPORTED_TARGETS darwin ios android raspberrypi linux tizen ios.framework)

if(NOT PLATFORM_TARGET)
  string(TOLOWER "${CMAKE_SYSTEM_NAME}" varplatform)
  string(TOUPPER "${CMAKE_SYSTEM_NAME}" VARPLATFORM)

  message(STATUS "No target defined (give as parameter -DPLATFORM_TARGET=platform_name).")
  message(STATUS "Targets available: ${SUPPORTED_TARGETS}.")
  message(STATUS "Will use your OS: ${varplatform}.")

  set(PLATFORM_TARGET ${varplatform})
else()
  string(TOLOWER "${PLATFORM_TARGET}" varplatform)
  string(TOUPPER "${PLATFORM_TARGET}" VARPLATFORM)

  message(STATUS "Platform chosen : ${VARPLATFORM}")
endif()

list(FIND SUPPORTED_TARGETS ${varplatform} target_in_list)

if(target_in_list EQUAL -1)
  message(SEND_ERROR "${varplatform} not in supported targets: ${SUPPORTED_TARGETS}")
  return()
endif()

# setting up cache variable platform
set(PLATFORM ${varplatform} CACHE INTERNAL "platform" FORCE)

# cmake output configuration
if(NOT ${PLATFORM_TARGET} STREQUAL android)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif()

if (CMAKE_BUILD_TYPE MATCHES Debug)
  add_definitions(-DDEBUG)
  add_definitions(-DLOG_LEVEL=3)
else()
  add_definitions(-DLOG_LEVEL=2)
endif()

set(EXTERNAL_LIBS_DIR ${CMAKE_SOURCE_DIR}/external/exports/${PLATFORM_TARGET}
    CACHE INTERNAL "external-libs" FORCE)

# Add platform-specific CMake file.
if(${varplatform} STREQUAL "darwin")
  add_definitions(-DTANGRAM_OSX)
  add_subdirectory(core)
  add_subdirectory(platforms/osx)
elseif(${varplatform} STREQUAL "ios")
  add_subdirectory(platforms/ios ios)
elseif(${varplatform} STREQUAL "android")
  add_subdirectory(platforms/android android)
elseif(${varplatform} STREQUAL "raspberrypi")
  add_subdirectory(platforms/rpi rpi)
elseif(${varplatform} STREQUAL "linux")
  add_subdirectory(platforms/linux linux)
elseif(${varplatform} STREQUAL "tizen")
  add_subdirectory(platforms/tizen tizen)
endif()

if(BENCHMARK OR UNIT_TESTS)
  add_library(platform_mock
    ${PROJECT_SOURCE_DIR}/tests/src/mockPlatform.cpp
    ${PROJECT_SOURCE_DIR}/tests/src/gl_mock.cpp
  )

  target_include_directories(platform_mock
    PUBLIC
    ${CORE_LIBRARIES_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/tests/src
  )

  target_compile_definitions(platform_mock
    PUBLIC
    -DUNIT_TESTS
  )

  target_link_libraries(platform_mock
    PUBLIC
    tangram-core
  )
endif()

if(UNIT_TESTS)
  message(STATUS "Build with tests")
  add_subdirectory(${PROJECT_SOURCE_DIR}/tests)
endif()

if(BENCHMARK)
  message(STATUS "Build with benchmarks")
  add_subdirectory(${PROJECT_SOURCE_DIR}/bench)
endif()
