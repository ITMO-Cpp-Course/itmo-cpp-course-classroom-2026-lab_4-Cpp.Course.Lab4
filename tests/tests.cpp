#include "file_handle.hpp"
#include "resource_manager.hpp"
#include <catch2/catch_all.hpp>
#include <cstdio>

using namespace lab4::resource;
const std::string TEST_FILE = "test_file.txt";

static void create_test_file()
{
    FILE* f = std::fopen(TEST_FILE.c_str(), "w");
    std::fclose(f);
}

static void remove_test_file()
{
    std::remove(TEST_FILE.c_str());
}

TEST_CASE("FileHandle opens existing file", "[file_handle]")
{
    create_test_file();
    FileHandle fh(TEST_FILE, "r");
    REQUIRE(fh.is_open());
    REQUIRE(fh.get() != nullptr);
    remove_test_file();
}

TEST_CASE("FileHandle throws on missing file", "[file_handle]")
{
    REQUIRE_THROWS_AS(FileHandle("nonexistent.txt", "r"), ResourceError);
}

TEST_CASE("FileHandle move transfers ownership", "[file_handle]")
{
    create_test_file();
    FileHandle a(TEST_FILE, "r");
    FileHandle b(std::move(a));
    REQUIRE(b.is_open());
    REQUIRE_FALSE(a.is_open());
    remove_test_file();
}

TEST_CASE("ResourceManager returns same handle for same file", "[resource_manager]")
{
    create_test_file();
    ResourceManager manager;
    auto a = manager.open(TEST_FILE, "r");
    auto b = manager.open(TEST_FILE, "r");
    REQUIRE(a.get() == b.get());
    remove_test_file();
}

TEST_CASE("ResourceManager reopens file after all handles released", "[resource_manager]")
{
    create_test_file();
    ResourceManager manager;
    FileHandle* raw = nullptr;
    {
        auto a = manager.open(TEST_FILE, "r");
        raw = a.get();
    } // a умер здесь
    auto b = manager.open(TEST_FILE, "r");
    REQUIRE(b.get() != raw);
    remove_test_file();
}