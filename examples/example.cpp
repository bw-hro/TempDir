// TempDir
// SPDX-FileCopyrightText: 2024-present Benno Waldhauer
// SPDX-License-Identifier: MIT

#include <bw/tempdir/tempdir.hpp>
#include <catch2/catch_all.hpp>
#include <fstream>

using namespace bw::tempdir;

// counts letters contained in a given file
struct LetterCounter
{
    std::filesystem::path file_path;
    int count()
    {
        std::ifstream file(file_path);
        if (!file.is_open())
            throw std::runtime_error("file missing " + file_path.string());

        std::ostringstream buffer;
        buffer << file.rdbuf();
        return buffer.str().length();
    }
};

TEST_CASE("Example how to use TempDir in unit tests", "[example]")
{
    TempDir temp_dir;

    auto txt_path = temp_dir.path() / "test.txt";
    LetterCounter letter_counter = {txt_path};

    SECTION("Reading before writing temporary file will throw")
    {
        REQUIRE_THROWS_AS(letter_counter.count(), std::runtime_error);
    }

    SECTION("Reading after writing temporary file will count letters")
    {
        std::ofstream file(txt_path);
        file << "Hello, world!";
        file.flush();

        REQUIRE(letter_counter.count() == 13);
    }
}
