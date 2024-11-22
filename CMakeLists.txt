cmake_minimum_required(VERSION 3.1)
cmake_policy(SET CMP0091 NEW)

message(STATUS "Read contents from vcpkg.json")
file(READ "${CMAKE_SOURCE_DIR}/vcpkg.json" VCPKG_JSON_CONTENT)

message(STATUS "Extract version from vcpkg.json")
string(REGEX MATCH "\"version-string\"[ \t]*:[ \t]*\"([^\"]+)\"" VERSION_MATCH "${VCPKG_JSON_CONTENT}")
if (VERSION_MATCH)
    string(REGEX REPLACE "\"version-string\"[ \t]*:[ \t]*\"([^\"]+)\"" "\\1" TEMP_DIR_VERSION "${VERSION_MATCH}")
    message(STATUS "Extracted version from vcpkg.json: ${TEMP_DIR_VERSION}")
else()
    message(FATAL_ERROR "Could not extract version from vcpkg.json")
endif()

project(TempDir VERSION ${TEMP_DIR_VERSION} DESCRIPTION "TempDir is a lightweight C++17 library designed to provide an easy-to-use solution for managing temporary directories and files," LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

if(WIN32)
    add_definitions(-DNOMINMAX)
endif(WIN32)

option(TD_BUILD_EXAMPLES "build examples" ON)
message("TD_BUILD_EXAMPLES: ${TD_BUILD_EXAMPLES}")
if(TD_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

option(TD_BUILD_TESTS "build tests" ON)
message("TD_BUILD_TESTS: ${TD_BUILD_TESTS}")
if(TD_BUILD_TESTS)
    add_subdirectory(test)
endif()

option(TD_ENABLE_COVERAGE "Enable coverage reporting" OFF)
message("TD_ENABLE_COVERAGE: ${TD_ENABLE_COVERAGE}")

add_library(tempdir INTERFACE)
target_include_directories(tempdir INTERFACE
    $<BUILD_INTERFACE:"${CMAKE_CURRENT_SOURCE_DIR}/include}"> 
    $<INSTALL_INTERFACE:include>
)

install(DIRECTORY include/ DESTINATION "${CMAKE_INSTALL_PREFIX}/include")
