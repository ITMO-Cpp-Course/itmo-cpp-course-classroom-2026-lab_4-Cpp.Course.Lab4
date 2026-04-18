#include <catch2/catch_all.hpp>

#include <cstdio>
#include <random>
#include <string>

#include "file_handle.hpp"
#include "resource_error.hpp"
#include "resource_manager.hpp"

using namespace lab4::resource;

namespace
{
std::string make_test_filename()
{
    static std::random_device rd;
    static std::minstd_rand gen(rd());
    static std::uniform_int_distribution<> dis(1000, 9999);
    return "tmp_" + std::to_string(dis(gen)) + ".dat";
}
} // namespace

TEST_CASE("ResourceError inherits from std::runtime_error", "[resource_error]")
{
    ResourceError err("test message");
    REQUIRE(std::string(err.what()) == "test message");

    bool caught = false;
    try
    {
        throw err;
    }
    catch (const std::runtime_error&)
    {
        caught = true;
    }
    REQUIRE(caught);
}

TEST_CASE("FileHandle: open non-existent file for reading throws", "[file_handle]")
{
    REQUIRE_THROWS_AS(FileHandle("/no/such/file/here", "r"), ResourceError);
}

TEST_CASE("FileHandle: write and read back data", "[file_handle]")
{
    std::string name = make_test_filename();
    std::string data = "Hello, RAII!";
    {
        FileHandle f(name, "w");
        f.write(data.data(), data.size());
    }
    {
        FileHandle f(name, "r");
        std::string buffer(data.size(), '\0');
        f.read(buffer.data(), buffer.size());
        REQUIRE(buffer == data);
    }
    std::remove(name.c_str());
}

TEST_CASE("FileHandle: move constructor transfers ownership", "[file_handle]")
{
    std::string name = make_test_filename();
    {
        FileHandle f1(name, "w");
        f1.write("test", 4);
        FileHandle f2(std::move(f1));
        REQUIRE(f2.is_open());
        f2.write(" more", 5);
    }
    {
        FileHandle f(name, "r");
        std::string buffer(9, '\0');
        f.read(buffer.data(), 9);
        REQUIRE(buffer == "test more");
    }
    std::remove(name.c_str());
}

TEST_CASE("FileHandle: move assignment transfers ownership", "[file_handle]")
{
    std::string name1 = make_test_filename();
    std::string name2 = make_test_filename();
    {
        FileHandle f1(name1, "w");
        f1.write("first", 5);
        FileHandle f2(name2, "w");
        f2.write("second", 6);
        f2 = std::move(f1);
        REQUIRE(f2.is_open());
        f2.write(" modified", 9);
    }
    {
        FileHandle f(name1, "r");
        std::string buffer(14, '\0');
        f.read(buffer.data(), 14);
        REQUIRE(buffer == "first modified");
    }
    std::remove(name1.c_str());
    std::remove(name2.c_str());
}

TEST_CASE("FileHandle: seek and read at different positions", "[file_handle]")
{
    std::string name = make_test_filename();
    {
        FileHandle f(name, "w");
        f.write("0123456789", 10);
    }
    {
        FileHandle f(name, "r");
        f.seek(5, SEEK_SET);
        char c;
        f.read(&c, 1);
        REQUIRE(c == '5');

        f.seek(2, SEEK_CUR);
        f.read(&c, 1);
        REQUIRE(c == '8');

        f.seek(-1, SEEK_END);
        f.read(&c, 1);
        REQUIRE(c == '9');
    }
    std::remove(name.c_str());
}

TEST_CASE("FileHandle: close makes is_open() false and double close throws", "[file_handle]")
{
    std::string name = make_test_filename();
    FileHandle f(name, "w");
    REQUIRE(f.is_open());
    f.close();
    REQUIRE_FALSE(f.is_open());
    REQUIRE_THROWS_AS(f.close(), ResourceError);
    std::remove(name.c_str());
}

TEST_CASE("FileHandle: operations on closed file throw", "[file_handle]")
{
    std::string name = make_test_filename();
    FileHandle f(name, "w");
    f.close();
    char buf[10];
    REQUIRE_THROWS_AS(f.read(buf, 1), ResourceError);
    REQUIRE_THROWS_AS(f.write("a", 1), ResourceError);
    REQUIRE_THROWS_AS(f.seek(0, SEEK_SET), ResourceError);
    std::remove(name.c_str());
}

TEST_CASE("ResourceManager: get caches and returns same object", "[resource_manager]")
{
    std::string name = make_test_filename();
    ResourceManager mgr;
    auto p1 = mgr.get(name, "w+");
    auto p2 = mgr.get(name, "w+");
    REQUIRE(p1.get() == p2.get());
    REQUIRE(mgr.active_count() == 1);
    p1.reset();
    p2.reset();
    mgr.cleanup();
    REQUIRE(mgr.active_count() == 0);
    std::remove(name.c_str());
}

TEST_CASE("ResourceManager: has_cached detects live objects", "[resource_manager]")
{
    std::string name = make_test_filename();
    ResourceManager mgr;
    {
        auto p = mgr.get(name, "w+");
        REQUIRE(mgr.has_cached(name));
    }
    REQUIRE_FALSE(mgr.has_cached(name));
    std::remove(name.c_str());
}

TEST_CASE("ResourceManager: evict removes entry from cache", "[resource_manager]")
{
    std::string name = make_test_filename();
    ResourceManager mgr;
    auto p = mgr.get(name, "w+");
    REQUIRE(mgr.has_cached(name));
    mgr.evict(name);
    REQUIRE_FALSE(mgr.has_cached(name));
    std::remove(name.c_str());
}

TEST_CASE("ResourceManager: expired resource is recreated", "[resource_manager]")
{
    std::string name = make_test_filename();
    ResourceManager mgr;
    void* addr1 = nullptr;
    {
        auto p1 = mgr.get(name, "w+");
        addr1 = p1.get();
    }
    auto p2 = mgr.get(name, "r");
    REQUIRE(p2.get() != addr1);
    std::remove(name.c_str());
}

TEST_CASE("ResourceManager: cleanup removes only expired weak_ptrs", "[resource_manager]")
{
    std::string name1 = make_test_filename();
    std::string name2 = make_test_filename();
    ResourceManager mgr;
    auto p1 = mgr.get(name1, "w+");
    {
        auto p2 = mgr.get(name2, "w+");
    }
    mgr.cleanup();
    REQUIRE(mgr.has_cached(name1));
    REQUIRE_FALSE(mgr.has_cached(name2));
    std::remove(name1.c_str());
    std::remove(name2.c_str());
}