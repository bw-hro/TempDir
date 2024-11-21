# TempDir: Simplified Temporary Directory for C++

[![CI Ubuntu](https://github.com/bw-hro/TempDir/actions/workflows/ubuntu.yml/badge.svg?branch=master)](https://github.com/bw-hro/TempDir/actions/workflows/ubuntu.yml)
[![CI Windows](https://github.com/bw-hro/TempDir/actions/workflows/windows.yml/badge.svg?branch=master)](https://github.com/bw-hro/TempDir/actions/workflows/windows.yml)
[![CI macOS](https://github.com/bw-hro/TempDir/actions/workflows/macos.yml/badge.svg?branch=master)](https://github.com/bw-hro/TempDir/actions/workflows/macos.yml)
[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/bw-hro/TempDir/master/LICENSE.txt)
[![GitHub Releases](https://img.shields.io/github/release/bw-hro/TempDir.svg)](https://github.com/bw-hro/TempDir/releases)
[![Vcpkg Version](https://img.shields.io/vcpkg/v/TempDir)](https://vcpkg.link/ports/TempDir)


**TempDir** is a lightweight C++17 library designed to provide an easy-to-use solution for managing temporary directories and files, particularly in unit testing scenarios (e.g., with [Catch2](https://github.com/catchorg/Catch2)). Inspired by JUnit's `@TempDir` annotation in the Java ecosystem, **TempDir** simplifies the creation, cleanup, and management of temporary directories in your C++ projects.

The library is implemented as a **single-header file**, making integration easy and comes without additional dependencies. The project is licensed under the **MIT License**.

## Features
- **Automatic cleanup**: Configurable cleanup policies to manage temporary directories' lifecycle.
- **Easy integration**: A single-header implementation that can be directly included in your project.
- **Unit test focus**: Perfect for use in testing frameworks like Catch2
- **Modern C++ support**: Written in C++17 for robust and efficient performance.

## Installation
Simply copy the [`tempdir.hpp`](include/bw/tempdir/tempdir.hpp) file into your project and include it in your source files:

```cpp
#include <bw/tempdir/tempdir.hpp>
```

## Example Usage

### Creating and Managing a Temporary Directory
```cpp
#include <bw/tempdir/tempdir.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>

using namespace bw::tempdir;

int main()
{
    try
    {
        TempDir temp_dir; // Creates a temporary directory in the system's temp folder.

        // Access the directory path
        std::cout << "Temporary directory created at: " << temp_dir.path() << std::endl;

        // Perform operations in the temp directory
        auto test_file = temp_dir.path() / "example.txt";
        std::ofstream(test_file) << "Hello, TempDir!";

        std::cout << "Created file: " << test_file << std::endl;

        // Directory is automatically cleaned up based on the cleanup policy
        // when temp_dir goes out of scope.
    }
    catch (const TempDirException& ex)
    {
        std::cerr << "An error occurred: " << ex.what() << std::endl;
    }

    return 0;
}
```
Please do also check [examples directory](/examples/) for more examples.

### Using TempDir in Catch2 Unit Tests
```cpp
#include <bw/tempdir/tempdir.hpp>
#include <catch2/catch_all.hpp>
#include <filesystem>
#include <fstream>

using namespace bw::tempdir;
namespace fs = std::filesystem;

TEST_CASE("TempDir usage in unit tests")
{
    TempDir temp_dir;

    REQUIRE(fs::exists(temp_dir.path())); // Temp directory should exist.

    auto test_file = temp_dir.path() / "test.txt";
    std::ofstream(test_file) << "Unit test data";

    REQUIRE(fs::exists(test_file)); // File should be created.
}
```


## Cleanup Policies
The `TempDir` class offers configurable cleanup policies:
- **`Cleanup::always`**: Always clean up the directory when `TempDir` goes out of scope.
- **`Cleanup::on_success`**: Clean up only if no exceptions were thrown.
- **`Cleanup::never`**: Keep the directory and its contents.

You can set the cleanup policy in the constructor:
```cpp
TempDir temp_dir(Cleanup::on_success);
```

## Logging
Disabled by default `TempDir` supports customizable logging by allowing you to provide a logging function in the `Config` object:
```cpp
// print to std::cout
TempDir temp_dir(Config().enable_logging());

// TempDir create '/tmp/temp_dir_1732162084442_37189'
// TempDir remove '/tmp/temp_dir_1732162084442_37189'


// forward to custom logger
TempDir temp_dir(Config().enable_logging([](auto& msg) { 
    spdlog::info(msg);
}));

```

## License
**TempDir** is licensed under the MIT License. See [LICENSE](LICENSE.txt) for details.

## Contributing
Contributions are welcome! If you encounter a bug, have a feature request, or would like to contribute code, feel free to open an issue or submit a pull request.
