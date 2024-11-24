// TempDir
// SPDX-FileCopyrightText: 2024-present Benno Waldhauer
// SPDX-License-Identifier: MIT

#include <bw/tempdir/tempdir.hpp>
#include <catch2/catch_all.hpp>

using namespace bw::tempdir;
namespace fs = std::filesystem;

constexpr bool is_win32 =
#ifdef _WIN32
    true;
#else
    false;
#endif

void restrict_directory_modification(const fs::path& dir_path)
{
    if constexpr (is_win32)
    {
        std::string everyone_lang_independent = "*S-1-1-0";
        std::string cmd = std::string("icacls ") + dir_path.string() + " /deny " +
                          everyone_lang_independent + ":(OI)(CI)(DE,DC,WDAC,WD,AD)";
        std::system(cmd.c_str());
    }
    else
    {
        fs::perms current_permissions = fs::status(dir_path).permissions();
        fs::permissions(dir_path, current_permissions & ~fs::perms::owner_write);
    }
}

// scope guard helper for clean up
struct ScopeGuard
{
    fs::path dir_path;

    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard& operator=(const ScopeGuard&) = delete;
    ~ScopeGuard() { cleanup(); }

    void cleanup()
    {
        if (fs::exists(dir_path))
        {
            if constexpr (is_win32)
            {
                std::string everyone_lang_independent = "*S-1-1-0";
                std::string cmd = std::string("icacls ") + dir_path.string() + " /remove:d " +
                                  everyone_lang_independent;
                std::system(cmd.c_str());
            }
            else
            {
                fs::permissions(dir_path, fs::perms::all);
            }
            fs::remove_all(dir_path);
        }
    };
};

TEST_CASE("TempDir can be moved but not copied")
{
    TempDir temp_dir_1;
    REQUIRE(fs::exists(temp_dir_1.path()));

    TempDir temp_dir_2 = std::move(temp_dir_1);
    REQUIRE(fs::exists(temp_dir_2.path()));
}

TEST_CASE("TempDir by default creates directory in OS temp directory")
{
    TempDir temp_dir;
    REQUIRE(fs::exists(temp_dir.path()));
    REQUIRE(fs::canonical(temp_dir.path().parent_path()) ==
            fs::canonical(fs::temp_directory_path()));
}

TEST_CASE("TempDirs root path is configurable")
{
    fs::path root_path = fs::temp_directory_path() / "my-custom-root";
    ScopeGuard{root_path};

    TempDir temp_dir_1(root_path);
    REQUIRE(fs::exists(temp_dir_1.path()));
    REQUIRE(temp_dir_1.path().parent_path() == root_path);

    TempDir temp_dir_2(root_path, Cleanup::on_success);
    REQUIRE(fs::exists(temp_dir_2.path()));
    REQUIRE(temp_dir_2.path().parent_path() == root_path);

    TempDir temp_dir_3(Config().set_root_path(root_path));
    REQUIRE(fs::exists(temp_dir_3.path()));
    REQUIRE(temp_dir_3.path().parent_path() == root_path);

    fs::remove_all(root_path);
}

TEST_CASE("TempDirs directory name prefix is configurable")
{
    TempDir temp_dir(Config().set_temp_dir_prefix("my-project"));
    REQUIRE(fs::exists(temp_dir.path()));

    std::string fn = temp_dir.path().filename().string();
    REQUIRE(fn.find("my-project") == 0);
}

TEST_CASE("TempDir throws exception when directory creation fails")
{
    fs::path root_path = fs::temp_directory_path() / "some-sub-dir";
    ScopeGuard{root_path};

    fs::create_directory(root_path);
    restrict_directory_modification(root_path);

    using namespace Catch::Matchers;
    REQUIRE_THROWS_MATCHES(TempDir(root_path), TempDirException,
                           MessageMatches(StartsWith("TempDirExcepton:")));
}

TEST_CASE("TempDir throws exception when directory cleanup fails")
{
    fs::path root_path = fs::temp_directory_path() / "some-sub-dir";
    ScopeGuard{root_path};

    TempDir temp_dir(root_path);
    REQUIRE(fs::is_directory(temp_dir.path()));

    restrict_directory_modification(root_path);

    using namespace Catch::Matchers;
    REQUIRE_THROWS_MATCHES(temp_dir.cleanup(), TempDirException,
                           MessageMatches(StartsWith("TempDirExcepton:")));
}

TEST_CASE("TempDir does not throw exception when cleanup in destructor fails")
{
    fs::path root_path = fs::temp_directory_path() / "some-sub-dir";
    ScopeGuard{root_path};
    fs::path temp_dir_path;

    { // scope that trigger destruction of temp_dir
        TempDir temp_dir(root_path);
        temp_dir_path = temp_dir.path();
        REQUIRE(fs::is_directory(temp_dir_path));

        restrict_directory_modification(root_path);
    }

    // dir still exists despite temp_dir tried to clean up, but no exception was thrown
    REQUIRE(fs::is_directory(temp_dir_path));
}

TEST_CASE("Temporary directory will 'always' be deleted when leaving scope")
{
    SECTION("Leave scope without exception")
    {
        fs::path temp_dir_path;
        {
            TempDir temp_dir(Cleanup::always);
            temp_dir_path = temp_dir.path();
            REQUIRE(fs::is_directory(temp_dir_path));
        }
        REQUIRE_FALSE(fs::is_directory(temp_dir_path));
        REQUIRE_FALSE(fs::exists(temp_dir_path));
    }

    SECTION("Leave scope with exception")
    {
        fs::path temp_dir_path;
        try
        {
            TempDir temp_dir(Cleanup::always);
            temp_dir_path = temp_dir.path();
            REQUIRE(fs::is_directory(temp_dir_path));
            throw std::runtime_error("some-expected-test-error");
        }
        catch (std::exception& ex)
        {
            REQUIRE_FALSE(fs::is_directory(temp_dir_path));
            REQUIRE_FALSE(fs::exists(temp_dir_path));
        }
    }
}

TEST_CASE("Temporary directory will 'never' be deleted when leaving scope")
{
    SECTION("Leave scope without exception")
    {
        fs::path temp_dir_path;
        {
            TempDir temp_dir(Cleanup::never);
            temp_dir_path = temp_dir.path();
            REQUIRE(fs::is_directory(temp_dir_path));
        }
        REQUIRE(fs::is_directory(temp_dir_path));
    }

    SECTION("Leave scope with exception")
    {
        fs::path temp_dir_path;
        try
        {
            TempDir temp_dir(Cleanup::never);
            temp_dir_path = temp_dir.path();
            REQUIRE(fs::is_directory(temp_dir_path));
            throw std::runtime_error("some-expected-test-error");
        }
        catch (std::exception& ex)
        {
            REQUIRE(fs::is_directory(temp_dir_path));
        }
    }
}

TEST_CASE("Temporary directory will be deleted 'on_success' when leaving scope")
{
    SECTION("Leave scope without exception")
    {
        fs::path temp_dir_path;
        {
            TempDir temp_dir(Cleanup::on_success);
            temp_dir_path = temp_dir.path();
            REQUIRE(fs::is_directory(temp_dir_path));
        }
        REQUIRE_FALSE(fs::is_directory(temp_dir_path));
        REQUIRE_FALSE(fs::exists(temp_dir_path));
    }

    SECTION("Leave scope with exception")
    {
        fs::path temp_dir_path;
        try
        {
            TempDir temp_dir(Cleanup::on_success);
            temp_dir_path = temp_dir.path();
            REQUIRE(fs::is_directory(temp_dir_path));
            throw std::runtime_error("some-expected-test-error");
        }
        catch (std::exception& ex)
        {
            REQUIRE(fs::is_directory(temp_dir_path));
        }
    }
}

TEST_CASE("TempDir allows to enable logging, which by default prints to std::cout")
{
    fs::path temp_dir_path;
    bool temp_dir_exists_in_scope = false;

    // redirect std::cout to the stringstream
    std::stringstream output;
    auto original_cout_buf = std::cout.rdbuf(output.rdbuf());

    { // scope that triggers destruction of temp_dir
        TempDir temp_dir(Config().enable_logging());
        temp_dir_path = temp_dir.path();
        temp_dir_exists_in_scope = fs::exists(temp_dir_path);
    }

    // restore the original std::cout buffer
    std::cout.flush();
    output.flush();
    std::cout.rdbuf(original_cout_buf);

    REQUIRE(temp_dir_exists_in_scope);
    REQUIRE_FALSE(fs::exists(temp_dir_path));

    std::vector<std::string> lines;
    std::string line;

    while (std::getline(output, line))
    {
        lines.push_back(line);
    }

    REQUIRE(lines.size() == 2);

    REQUIRE(lines[0].find("TempDir create") != std::string::npos);
    REQUIRE(lines[0].find(temp_dir_path.string()) != std::string::npos);

    REQUIRE(lines[1].find("TempDir remove") != std::string::npos);
    REQUIRE(lines[1].find(temp_dir_path.string()) != std::string::npos);
}

TEST_CASE("TempDir allows to enable custom logging")
{
    fs::path temp_dir_path;
    std::vector<std::string> log;

    { // scope that triggers destruction of temp_dir
        TempDir temp_dir(Config().enable_logging([&](auto& msg) { log.push_back(msg); }));
        temp_dir_path = temp_dir.path();
        REQUIRE(fs::exists(temp_dir_path));
    }
    REQUIRE_FALSE(fs::exists(temp_dir_path));

    REQUIRE(log[0].find("TempDir create") != std::string::npos);
    REQUIRE(log[0].find(temp_dir_path.string()) != std::string::npos);

    REQUIRE(log[1].find("TempDir remove") != std::string::npos);
    REQUIRE(log[1].find(temp_dir_path.string()) != std::string::npos);
}
