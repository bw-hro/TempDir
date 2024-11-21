cmake_minimum_required(VERSION 3.1)
cmake_policy(SET CMP0091 NEW)

project(TempDir VERSION 1.0 DESCRIPTION "TempDir is a lightweight C++17 library designed to provide an easy-to-use solution for managing temporary directories and files," LANGUAGES CXX)
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

add_library(tempdir INTERFACE)
target_include_directories(tempdir INTERFACE
    $<BUILD_INTERFACE:"${CMAKE_CURRENT_SOURCE_DIR}/include}"> 
    $<INSTALL_INTERFACE:include>
)

install(DIRECTORY include/ DESTINATION "${CMAKE_INSTALL_PREFIX}/include")
