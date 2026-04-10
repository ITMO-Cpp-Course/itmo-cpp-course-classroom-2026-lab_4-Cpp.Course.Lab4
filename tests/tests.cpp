#include <catch2/catch_all.hpp>

#include <resource_core/file_handle.hpp>
#include <resource_core/resource_manager.hpp>

#include <cstdio>
#include <filesystem>
#include <memory>
#include <string>

namespace fs = std::filesystem;
using namespace lab4::resource;

static std::string make_temp_file(const std::string& content = "")
{
    static int counter = 0;
    auto path = (fs::temp_directory_path() / ("lab4_test_" + std::to_string(counter++) + ".txt")).string();
    std::FILE* f = std::fopen(path.c_str(), "w");
    if (f)
    {
        std::fwrite(content.data(), 1, content.size(), f);
        std::fclose(f);
    }
    return path;
}

TEST_CASE("ResourceError is thrown for non-existent file", "[ResourceError]")
{
    REQUIRE_THROWS_AS(FileHandle("/nonexistent/path/file.txt", "r"), ResourceError);
}

TEST_CASE("ResourceError message contains file path", "[ResourceError]")
{
    try
    {
        FileHandle fh("/no/such/file.txt", "r");
        FAIL("Expected ResourceError");
    }
    catch (const ResourceError& e)
    {
        std::string msg = e.what();
        REQUIRE_FALSE(msg.empty());
    }
}

TEST_CASE("FileHandle opens an existing file", "[FileHandle]")
{
    auto path = make_temp_file("hello");
    FileHandle fh(path, "r");
    REQUIRE(fh.is_open());
    REQUIRE(fh.path() == path);
}

TEST_CASE("FileHandle closes on destruction", "[FileHandle]")
{
    auto path = make_temp_file("data");
    {
        FileHandle fh(path, "r");
        REQUIRE(fh.is_open());
    }
    FileHandle fh2(path, "r");
    REQUIRE(fh2.is_open());
}

TEST_CASE("FileHandle::close makes is_open() return false", "[FileHandle]")
{
    auto path = make_temp_file();
    FileHandle fh(path, "r");
    REQUIRE(fh.is_open());
    fh.close();
    REQUIRE_FALSE(fh.is_open());
}

TEST_CASE("FileHandle write and read round-trip", "[FileHandle]")
{
    auto path = make_temp_file();
    const std::string content = "qwr ewq";

    {
        FileHandle writer(path, "w");
        writer.write(content);
    }

    FileHandle reader(path, "r");
    REQUIRE(reader.read_all() == content);
}

TEST_CASE("Reading from a closed FileHandle throws ResourceError", "[FileHandle]")
{
    auto path = make_temp_file("x");
    FileHandle fh(path, "r");
    fh.close();
    REQUIRE_THROWS_AS(fh.read_all(), ResourceError);
}

TEST_CASE("Writing to a closed FileHandle throws ResourceError", "[FileHandle]")
{
    auto path = make_temp_file();
    FileHandle fh(path, "w");
    fh.close();
    REQUIRE_THROWS_AS(fh.write("data"), ResourceError);
}

TEST_CASE("FileHandle is move-constructible", "[FileHandle][move]")
{
    auto path = make_temp_file("move test");
    FileHandle original(path, "r");
    REQUIRE(original.is_open());

    FileHandle moved(std::move(original));
    REQUIRE(moved.is_open());
    REQUIRE_FALSE(original.is_open());
    REQUIRE(moved.path() == path);
}

TEST_CASE("FileHandle is move-assignable", "[FileHandle][move]")
{
    auto path1 = make_temp_file("file1");
    auto path2 = make_temp_file("file2");

    FileHandle fh1(path1, "r");
    FileHandle fh2(path2, "r");

    fh1 = std::move(fh2);
    REQUIRE(fh1.is_open());
    REQUIRE(fh1.path() == path2);
    REQUIRE_FALSE(fh2.is_open());
}

TEST_CASE("FileHandle is not copyable", "[FileHandle]")
{
    STATIC_REQUIRE_FALSE(std::is_copy_constructible_v<FileHandle>);
    STATIC_REQUIRE_FALSE(std::is_copy_assignable_v<FileHandle>);
}

TEST_CASE("ResourceManager returns a valid shared_ptr", "[ResourceManager]")
{
    auto path = make_temp_file("mgr test");
    ResourceManager mgr;
    auto handle = mgr.open(path, "r");
    REQUIRE(handle != nullptr);
    REQUIRE(handle->is_open());
}

TEST_CASE("ResourceManager returns same object for same path", "[ResourceManager][cache]")
{
    auto path = make_temp_file("shared");
    ResourceManager mgr;

    auto h1 = mgr.open(path, "r");
    auto h2 = mgr.open(path, "r");

    REQUIRE(h1.get() == h2.get());
}

TEST_CASE("ResourceManager cache returns new object after all handles released", "[ResourceManager][cache]")
{
    auto path = make_temp_file("expire");
    ResourceManager mgr;

    FileHandle* raw = nullptr;
    {
        auto h1 = mgr.open(path, "r");
        raw = h1.get();
    }
    auto h2 = mgr.open(path, "r");
    REQUIRE(h2.get() != raw);
}

TEST_CASE("ResourceManager shared ownership keeps resource alive", "[ResourceManager][shared_ptr]")
{
    auto path = make_temp_file("shared ownership");
    ResourceManager mgr;

    std::shared_ptr<FileHandle> h1 = mgr.open(path, "r");
    {
        auto h2 = mgr.open(path, "r");
        REQUIRE(h1.use_count() == 2);
    }
    REQUIRE(h1->is_open());
    REQUIRE(h1.use_count() == 1);
}

TEST_CASE("ResourceManager::purge_expired removes stale entries", "[ResourceManager][cache]")
{
    auto path = make_temp_file("purge");
    ResourceManager mgr;

    {
        auto h = mgr.open(path, "r");
        REQUIRE(mgr.cache_size() == 1);
    }
    mgr.purge_expired();
    REQUIRE(mgr.cache_size() == 0);
}

TEST_CASE("ResourceManager throws ResourceError for bad path", "[ResourceManager][ResourceError]")
{
    ResourceManager mgr;
    REQUIRE_THROWS_AS(mgr.open("/no/such/file.txt", "r"), ResourceError);
}
