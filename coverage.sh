#!/usr/bin/env bash

script_path=$(realpath "$0")
base_dir="$( dirname "$script_path" )"
build_dir="$base_dir/build"

${base_dir}/build.sh clean debug

echo "generate coverage.info:"
lcov --capture --directory . --output-file "${build_dir}/coverage.info"

echo "generate filtered_coverage.info:"
lcov --extract "${build_dir}/coverage.info" '*/bw/tempdir/*' --output-file "${build_dir}/filtered_coverage.info"

echo "generate coverage report:"
rm -R "${build_dir}/coverage_report"
genhtml "${build_dir}/filtered_coverage.info" --output-directory "${build_dir}/coverage_report"
