#include "resource/FileHandle.hpp"
#include "resource/ResourceError.hpp"
#include "resource/ResourceManager.hpp"
#include <catch2/catch_all.hpp>

#include <cstdio>
#include <filesystem>

namespace fs = std::filesystem;

static void try_remove(const char* path)
{
    if (fs::exists(path))
        fs::remove(path);
}

TEST_CASE("ResourceError stores message", "[ResourceError]")
{
    lab4::resource::ResourceError err("something went wrong");
    REQUIRE(std::string(err.what()) == "something went wrong");
}

TEST_CASE("ResourceError is std::runtime_error", "[ResourceError]")
{
    try
    {
        throw lab4::resource::ResourceError("error");
    }
    catch (const std::runtime_error& err)
    {
        REQUIRE(std::string(err.what()) == "error");
    }
}

TEST_CASE("ResourceError from char*", "[ResourceError]")
{
    REQUIRE_THROWS_AS([] { throw lab4::resource::ResourceError("c-string error"); }(), lab4::resource::ResourceError);
}

TEST_CASE("Default FileHandle is not open", "[FileHandle]")
{
    lab4::resource::FileHandle fh;
    REQUIRE_FALSE(fh.is_open());
}

TEST_CASE("FileHandle open and close", "[FileHandle]")
{
    const char* path = "test_open_close.txt";

    {
        lab4::resource::FileHandle fh(path, "w");
        REQUIRE(fh.is_open());
        REQUIRE(fh.path() == path);
        fh.close();
    }
    try_remove(path);
}

TEST_CASE("FileHandle throws on missing file", "[FileHandle]")
{
    REQUIRE_THROWS_AS(lab4::resource::FileHandle("no_such_file.txt", "r"), lab4::resource::ResourceError);
}

TEST_CASE("FileHandle throws on double open", "[FileHandle]")
{
    const char* path = "test_double_open.txt";

    {
        lab4::resource::FileHandle fh(path, "w");
        REQUIRE_THROWS_AS(fh.open(path, "w"), lab4::resource::ResourceError);
    }
    try_remove(path);
}

TEST_CASE("FileHandle throws on close when not open", "[FileHandle]")
{
    lab4::resource::FileHandle fh;
    REQUIRE_THROWS_AS(fh.close(), lab4::resource::ResourceError);
}

TEST_CASE("FileHandle move constructor", "[FileHandle]")
{
    const char* path = "test_move_ctor.txt";

    {
        lab4::resource::FileHandle fh1(path, "w");
        REQUIRE(fh1.is_open());

        lab4::resource::FileHandle fh2(std::move(fh1));
        REQUIRE_FALSE(fh1.is_open());
        REQUIRE(fh2.is_open());
        fh2.close();
    }
    try_remove(path);
}

TEST_CASE("FileHandle move assignment", "[FileHandle]")
{
    const char* path1 = "test_move_a1.txt";
    const char* path2 = "test_move_a2.txt";

    {
        lab4::resource::FileHandle fh1(path1, "w");
        lab4::resource::FileHandle fh2(path2, "w");

        fh1.close();
        fh2 = std::move(fh1);
        REQUIRE_FALSE(fh1.is_open());
    }
    try_remove(path1);
    try_remove(path2);
}

TEST_CASE("FileHandle write and read", "[FileHandle]")
{
    const char* path = "test_write_read.txt";

    {
        lab4::resource::FileHandle fh(path, "w");
        fh.write("Hello, RAII!");
    }

    {
        lab4::resource::FileHandle fh(path, "r");
        REQUIRE(fh.read_all() == "Hello, RAII!");
    }

    try_remove(path);
}

TEST_CASE("FileHandle throws on write to read-only file", "[FileHandle]")
{
    const char* path = "test_readonly.txt";

    {
        lab4::resource::FileHandle w(path, "w");
        w.write("data");
    }

    {
        lab4::resource::FileHandle r(path, "r");
        REQUIRE_THROWS_AS(r.write("nope"), lab4::resource::ResourceError);
    }

    try_remove(path);
}

TEST_CASE("FileHandle get() returns raw FILE*", "[FileHandle]")
{
    const char* path = "test_raw_ptr.txt";

    {
        lab4::resource::FileHandle fh(path, "w");
        REQUIRE(fh.get() != nullptr);

        fh.write("via get() test");

        std::fflush(fh.get());
    }

    {
        lab4::resource::FileHandle fh(path, "r");
        REQUIRE(fh.read_all() == "via get() test");
    }

    try_remove(path);
}

TEST_CASE("ResourceManager acquire creates new resource", "[ResourceManager]")
{
    const char* path = "mgr_acquire.txt";

    {
        lab4::resource::ResourceManager mgr;
        auto fh = mgr.acquire(path, "w");
        REQUIRE(fh->is_open());
        REQUIRE(mgr.contains(path));
    }
    try_remove(path);
}

TEST_CASE("ResourceManager returns same instance from cache", "[ResourceManager]")
{
    const char* path = "mgr_cache.txt";

    {
        lab4::resource::ResourceManager mgr;
        auto fh1 = mgr.acquire(path, "w");
        auto fh2 = mgr.acquire(path, "w");
        REQUIRE(fh1 == fh2);
    }
    try_remove(path);
}

TEST_CASE("ResourceManager cache_size", "[ResourceManager]")
{
    const char* p1 = "mgr_size1.txt";
    const char* p2 = "mgr_size2.txt";

    {
        lab4::resource::ResourceManager mgr;
        auto fh1 = mgr.acquire(p1, "w");
        REQUIRE(fh1->is_open());
        auto fh2 = mgr.acquire(p2, "w");
        REQUIRE(fh2->is_open());

        REQUIRE(mgr.cache_size() == 2);
    }
    try_remove(p1);
    try_remove(p2);
}

TEST_CASE("ResourceManager cleanup removes expired", "[ResourceManager]")
{
    const char* path = "mgr_cleanup.txt";

    {
        lab4::resource::ResourceManager mgr;
        {
            auto fh = mgr.acquire(path, "w");
            REQUIRE(mgr.cache_size() == 1);
        }
        mgr.cleanup();
        REQUIRE(mgr.cache_size() == 0);
    }
    try_remove(path);
}

TEST_CASE("ResourceManager release removes resource", "[ResourceManager]")
{
    const char* path = "mgr_release.txt";

    {
        lab4::resource::ResourceManager mgr;
        auto fh = mgr.acquire(path, "w");
        REQUIRE(fh->is_open());
        REQUIRE(mgr.contains(path));

        mgr.release(path);
        REQUIRE_FALSE(mgr.contains(path));
    }
    try_remove(path);
}

TEST_CASE("ResourceManager multiple files", "[ResourceManager]")
{
    const char* p1 = "mgr_multi1.txt";
    const char* p2 = "mgr_multi2.txt";
    const char* p3 = "mgr_multi3.txt";

    {
        lab4::resource::ResourceManager mgr;
        auto fh1 = mgr.acquire(p1, "w");
        auto fh2 = mgr.acquire(p2, "w");
        auto fh3 = mgr.acquire(p3, "w");

        fh1->write("file1");
        fh2->write("file2");
        fh3->write("file3");

        REQUIRE(mgr.cache_size() == 3);
        REQUIRE(fh1 != fh2);
        REQUIRE(fh2 != fh3);
    }
    try_remove(p1);
    try_remove(p2);
    try_remove(p3);
}

TEST_CASE("ResourceManager shared access", "[ResourceManager]")
{
    const char* path = "mgr_shared.txt";

    {
        lab4::resource::ResourceManager mgr;
        auto fh1 = mgr.acquire(path, "w");
        fh1->write("shared data");

        auto fh2 = mgr.acquire(path, "w");
        REQUIRE(fh1 == fh2);
    }
    try_remove(path);
}

TEST_CASE("ResourceManager contains after cleanup", "[ResourceManager]")
{
    const char* path = "mgr_contains_after.txt";

    {
        lab4::resource::ResourceManager mgr;
        {
            auto fh = mgr.acquire(path, "w");
            REQUIRE(mgr.contains(path));
        }
        mgr.cleanup();
        REQUIRE_FALSE(mgr.contains(path));
    }
    try_remove(path);
}
