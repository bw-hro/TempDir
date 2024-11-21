@echo off

set "script_path=%~f0"
set "base_dir=%~dp0"
set "build_dir=%base_dir%build"
set "toolchain_file=%base_dir%tools\vcpkg\scripts\buildsystems\vcpkg.cmake"

echo %* | find /i "clean" > nul
if %errorlevel% equ 0 (
    echo Cleaning project - removing %build_dir%
    rmdir /s /q %build_dir%
) else (
    echo Using cache for project build
)

echo %* | find /i "release" > nul
if %errorlevel% equ 0 (
    set "build_type=Release"
) else (
    set "build_type=Debug"
)
echo Project build type: %build_type%

echo %* | find /i "Win32" > nul
if %errorlevel% equ 0 (
    set "build_arch=Win32"
    set "vcpkg_triplet=x86-windows-static"
) else (
    set "build_arch=x64"
    set "vcpkg_triplet=x64-windows-static"
)
echo Project architecture: %build_arch%

set "vcpkg_file=vcpkg.json"

cmake -B "%build_dir%" -S . -DCMAKE_BUILD_TYPE=%build_type% -DCMAKE_TOOLCHAIN_FILE="%toolchain_file%" -DVCPKG_TARGET_TRIPLET="%vcpkg_triplet%" -G "Visual Studio 17 2022" -A "%build_arch%"
cmake --build "%build_dir%" --config "%build_type%" --parallel %NUMBER_OF_PROCESSORS%

ctest --test-dir "%build_dir%\test\"
ctest --test-dir "%build_dir%\examples\"
