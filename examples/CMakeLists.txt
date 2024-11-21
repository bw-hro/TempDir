find_package(Catch2 3 REQUIRED)

add_executable(example "example.cpp")

set_property(TARGET example PROPERTY
             MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set(INCLUDES_FOR_EXAMPLE ../include)
target_compile_definitions(example PRIVATE CATCH_CONFIG_ENABLE_ALL_STRINGMAKERS)
target_include_directories(example PRIVATE ${INCLUDES_FOR_EXAMPLE})
target_link_libraries(example PRIVATE Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(example tests)