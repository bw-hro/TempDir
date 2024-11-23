find_package(Catch2 3 REQUIRED)

add_executable(tests
    "catch2/unit_tests/tempdir_tests.cpp"
)

set_property(TARGET tests PROPERTY
             MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set(INCLUDES_FOR_TESTS ../include)
target_compile_definitions(tests PRIVATE CATCH_CONFIG_ENABLE_ALL_STRINGMAKERS)
target_include_directories(tests PRIVATE ${INCLUDES_FOR_TESTS})
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)

if(TD_ENABLE_COVERAGE)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(tests PRIVATE --coverage)
        target_link_options(tests PRIVATE --coverage)
    else()
        message(FATAL_ERROR "Coverage only supported with GCC/Clang")
    endif()
endif()

include(CTest)
include(Catch)
catch_discover_tests(tests)
