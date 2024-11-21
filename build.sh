#!/usr/bin/env bash

script_path=$(realpath "$0")
base_dir="$( dirname "$script_path" )"
build_dir="$base_dir/build"
toolchain_file="$base_dir/tools/vcpkg/scripts/buildsystems/vcpkg.cmake"

if [[ "${@#clean}" = "$@" ]]
then
    echo "use cache for project build"
else
    echo "clean project - remove $build_dir"
    rm -rf $build_dir
fi

if [[ "${@#release}" = "$@" ]]
then
    build_type="Debug"
else
    build_type="Release"
    rm -rf $build_dir
fi
echo "project build type: $build_type"

vcpkg_file="vcpkg.json"

cmake -B build -S . -D"CMAKE_BUILD_TYPE=$build_type" -D"CMAKE_TOOLCHAIN_FILE=$toolchain_file" -D"CMAKE_MAKE_PROGRAM:PATH=make" -D"CMAKE_CXX_COMPILER=g++"
cmake --build build --parallel $(nproc)

ctest --test-dir build/test/
ctest --test-dir build/examples/
