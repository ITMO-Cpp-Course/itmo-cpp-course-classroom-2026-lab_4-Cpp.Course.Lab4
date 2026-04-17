
#include "file_handle.hpp"
#include "resource_error.hpp"
#include "resource_manager.hpp"
#include <catch2/catch_all.hpp>
#include <cstdio>
using namespace lab4::resource;

void prepare_file(const char* name)
{
    FILE* f = std::fopen(name, "w");
    if (f)
    {
        std::fclose(f);
    }
}

TEST_CASE("Testing ResourceError")
{
    try
    {
        throw ResourceError("Error occurred!");
    }
    catch (const ResourceError& e)
    {
        std::string msg = e.what();
        REQUIRE(msg == "Error occurred!");
    }
}
TEST_CASE("ResourceError inherits from std::runtime_error")
{
    try
    {
        throw ResourceError("test");
    }
    catch (const std::runtime_error&)
    {
        REQUIRE(true);
    }
    catch (...)
    {
        REQUIRE(false);
    }
}

TEST_CASE("FileHandle: ()")
{
    const char* fname = "test_open_close.txt";
    prepare_file(fname);

    FileHandle f;
    REQUIRE(!f.is_open());

    f.open(fname, "r");
    REQUIRE(f.is_open());
    REQUIRE(f.is_open());

    f.close();
    REQUIRE(!f.is_open());

    std::remove(fname);
}

TEST_CASE("FileHandle: constructor with parameters")
{
    const char* fname = "test_ctor.txt";
    prepare_file(fname);

    FileHandle f(fname, "r");
    REQUIRE(f.is_open());

    std::remove(fname);
}
TEST_CASE("FileHandle: Destructor")
{
    const char* fname = "test_raii.txt";
    prepare_file(fname);

    {
        FileHandle f(fname, "r");
        REQUIRE(f.is_open());
    }
    REQUIRE(std::remove(fname) == 0);
}

TEST_CASE("FileHandle: copy prohibition")
{
    static_assert(!std::is_copy_constructible_v<FileHandle>);
    static_assert(!std::is_copy_assignable_v<FileHandle>);
    REQUIRE(true);
}

TEST_CASE("FileHandle: move-constructor ")
{
    const char* fname = "test_move_ctor.txt";
    prepare_file(fname);

    FileHandle f1(fname, "r");
    FileHandle f2 = std::move(f1);
    REQUIRE(!f1.is_open());
    REQUIRE(f2.is_open());

    std::remove(fname);
}

TEST_CASE("FileHandle: move-assignment ")
{
    const char* fname = "test_move_assign.txt";
    prepare_file(fname);

    FileHandle f1(fname, "r");
    FileHandle f2;
    f2 = std::move(f1);
    REQUIRE(!f1.is_open());
    REQUIRE(f2.is_open());

    std::remove(fname);
}

TEST_CASE("FileHandle: Nonexistent fail")
{
    try
    {
        FileHandle f("non_existent_file_12345.txt", "r");
        REQUIRE(false);
    }
    catch (const ResourceError& e)
    {
        REQUIRE(std::string(e.what()).find("Failed to open file") != std::string::npos);
    }
}

TEST_CASE("ResourceManager: Caching same file")
{
    const char* filename = "manager_test_cache.txt";
    prepare_file(filename);
    ResourceManager rm;

    auto ptr1 = rm.get(filename, "r");
    auto ptr2 = rm.get(filename, "r");

    REQUIRE(ptr1 == ptr2);
    REQUIRE(ptr1.use_count() >= 2);

    std::remove(filename);
}

TEST_CASE("ResourceManager: Cleaning the cache")
{
    const char* filename = "manager_test_remove.txt";
    prepare_file(filename);
    ResourceManager rm;

    auto ptr1 = rm.get(filename, "r");
    rm.remove(filename);
    auto ptr2 = rm.get(filename, "r");

    REQUIRE(ptr1 != ptr2);

    std::remove(filename);
}

TEST_CASE("ResourceManager: Automatic cleanup when unused")
{
    const char* filename = "manager_test_weak.txt";
    prepare_file(filename);
    ResourceManager rm;

    std::weak_ptr<FileHandle> observer;

    {
        auto ptr = rm.get(filename, "r");
        observer = ptr;
        REQUIRE(observer.expired() == false);
    }

    REQUIRE(observer.expired() == true);

    std::remove(filename);
}