#include "file_handle.hpp"
#include "resource_error.hpp"
#include "resource_manager.hpp"
#include <catch2/catch_all.hpp>
#include <cstdio>
#include <string>

using namespace lab4::resource;

namespace
{
int file_counter = 0;
std::string unique_name()
{
    ++file_counter;
    return "test_file_" + std::to_string(file_counter) + ".txt";
}
} // namespace

TEST_CASE("ResourceError inherits from std::runtime_error", "[resource_error]")
{
    ResourceError err("some error");
    REQUIRE(std::string(err.what()) == "some error");
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

TEST_CASE("FileHandle: open non-existent throws", "[file_handle]")
{
    REQUIRE_THROWS_AS(FileHandle("/no/such/file", "r"), ResourceError);
}

TEST_CASE("FileHandle: write and read", "[file_handle]")
{
    auto name = unique_name();
    std::string data = "Hello, World!";
    {
        FileHandle f(name, "w");
        f.write(data.data(), data.size());
    }
    {
        FileHandle f(name, "r");
        std::string buf(data.size(), '\0');
        f.read(buf.data(), buf.size());
        REQUIRE(buf == data);
    }
    std::remove(name.c_str());
}

TEST_CASE("FileHandle: move constructor", "[file_handle]")
{
    auto name = unique_name();
    FileHandle f1(name, "w");
    f1.write("abc", 3);
    FileHandle f2(std::move(f1));
    f2.write("def", 3);
    f2.close();
    {
        FileHandle f(name, "r");
        char buf[7] = {0};
        f.read(buf, 6);
        REQUIRE(std::string(buf) == "abcdef");
    }
    std::remove(name.c_str());
}

TEST_CASE("FileHandle: move assignment", "[file_handle]")
{
    auto name1 = unique_name();
    auto name2 = unique_name();
    FileHandle f1(name1, "w");
    f1.write("111", 3);
    FileHandle f2(name2, "w");
    f2.write("222", 3);
    f2 = std::move(f1);
    f2.write("333", 3);
    f2.close();
    {
        FileHandle f(name1, "r");
        char buf[7] = {0};
        f.read(buf, 6);
        REQUIRE(std::string(buf) == "111333");
    }
    std::remove(name1.c_str());
    std::remove(name2.c_str());
}

TEST_CASE("FileHandle: seek", "[file_handle]")
{
    auto name = unique_name();
    {
        FileHandle f(name, "w");
        f.write("0123456789", 10);
    }
    {
        FileHandle f(name, "r");
        char c;
        f.seek(5, SEEK_SET);
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

TEST_CASE("FileHandle: close and double close", "[file_handle]")
{
    auto name = unique_name();
    FileHandle f(name, "w");
    REQUIRE(f.is_open());
    f.close();
    REQUIRE_FALSE(f.is_open());
    REQUIRE_THROWS_AS(f.close(), ResourceError);
    std::remove(name.c_str());
}

TEST_CASE("FileHandle: operations on closed file throw", "[file_handle]")
{
    auto name = unique_name();
    FileHandle f(name, "w");
    f.close();
    char data[10];
    REQUIRE_THROWS_AS(f.read(data, 1), ResourceError);
    REQUIRE_THROWS_AS(f.write("x", 1), ResourceError);
    REQUIRE_THROWS_AS(f.seek(0, SEEK_SET), ResourceError);
    std::remove(name.c_str());
}

TEST_CASE("ResourceManager: get caches same object", "[resource_manager]")
{
    auto name = unique_name();
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

TEST_CASE("ResourceManager: has_cached works", "[resource_manager]")
{
    auto name = unique_name();
    ResourceManager mgr;
    {
        auto p = mgr.get(name, "w+");
        REQUIRE(mgr.has_cached(name));
    }
    REQUIRE_FALSE(mgr.has_cached(name));
    std::remove(name.c_str());
}

TEST_CASE("ResourceManager: evict removes entry", "[resource_manager]")
{
    auto name = unique_name();
    ResourceManager mgr;
    auto p = mgr.get(name, "w+");
    REQUIRE(mgr.has_cached(name));
    mgr.evict(name);
    REQUIRE_FALSE(mgr.has_cached(name));
    std::remove(name.c_str());
}

TEST_CASE("ResourceManager: expired resource is recreated", "[resource_manager]")
{
    auto name = unique_name();
    ResourceManager mgr;
    {
        auto p = mgr.get(name, "w+");
        p->write("first", 5);
    }
    {
        auto p = mgr.get(name, "w+");
        p->write("second", 6);
    }
    {
        FileHandle f(name, "r");
        char buf[7] = {0};
        f.read(buf, 6);
        REQUIRE(std::string(buf) == "second");
    }
    std::remove(name.c_str());
}

TEST_CASE("ResourceManager: cleanup removes only expired", "[resource_manager]")
{
    auto name1 = unique_name();
    auto name2 = unique_name();
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