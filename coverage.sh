#!/usr/bin/env bash

script_path=$(realpath "$0")
base_dir="$( dirname "$script_path" )"
build_dir="$base_dir/build"

${base_dir}/build.sh clean debug

echo "generate coverage.info:"
lcov --capture --directory . --output-file "${build_dir}/coverage.info"

echo "generate filtered_coverage.info:"
lcov --extract "${build_dir}/coverage.info" '*/bw/tempdir/*' --output-file "${build_dir}/filtered_coverage.info"

echo "generate coverage_summary.json"
lcov --summary "${build_dir}/filtered_coverage.info" > "${build_dir}/coverage_summary.txt"
total_line_coverage=$(grep -E "lines" "${build_dir}/coverage_summary.txt" | awk '{print $2}')
echo "{\"total_line_coverage\": \"$total_line_coverage\"}" > "${build_dir}/coverage_summary.json"

echo "generate coverage report:"
rm -Rf "${build_dir}/coverage_report"
genhtml "${build_dir}/filtered_coverage.info" --output-directory "${build_dir}/coverage_report"

echo "download coverage badge"
curl -o "${build_dir}/coverage_report/badge.svg" "https://img.shields.io/badge/coverage-${total_line_coverage}25-rgb%2850,201,85%29"
