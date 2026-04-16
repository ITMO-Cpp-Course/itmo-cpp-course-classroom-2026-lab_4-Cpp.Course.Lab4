#define _CRT_SECURE_NO_WARNINGS
#include <catch2/catch_all.hpp>
#include <cstdio>
#include <type_traits>

#include "handle.hpp"
#include "resource_error.hpp"
#include "resource_manager.hpp"
using namespace lab4::resource;

void createFile(const std::string& name, const std::string& content = "")
{
    std::FILE* f = std::fopen(name.c_str(), "w");
    if (f)
    {
        std::fwrite(content.c_str(), 1, content.size(), f);
        std::fclose(f);
    }
}
void deleteFile(const std::string& name)
{
    std::remove(name.c_str());
}

TEST_CASE("Create and delete", "[FileHandle]")
{
    const std::string name = "test.txt";
    createFile(name);
    SECTION("create")
    {
        {
            FileHandle f(name, "r");
            REQUIRE(f.is_open());
        }
        REQUIRE_NOTHROW(FileHandle(name, "r"));
    }
    deleteFile(name);
}

TEST_CASE("Copying is prohibited", "[FileHandle]")
{
    REQUIRE_FALSE(std::is_copy_constructible<FileHandle>::value);
    REQUIRE_FALSE(std::is_copy_assignable<FileHandle>::value);
}

TEST_CASE("Moving is allowed", "[FileHandle]")
{
    const std::string name = "test.txt";
    createFile(name);
    FileHandle f1(name, "r");
    FileHandle f2 = std::move(f1);
    REQUIRE(f1.is_open() == false);
    REQUIRE(f2.is_open() == true);
    deleteFile(name);
}

TEST_CASE("Open and close", "[FileHandle]")
{
    const std::string name = "test.txt";
    createFile(name);
    FileHandle f;
    REQUIRE(f.is_open() == false);

    f.open(name, "r");
    REQUIRE(f.is_open() == true);

    f.close();
    REQUIRE(f.is_open() == false);

    deleteFile(name);
}

TEST_CASE("Error", "[FileHandle]")
{
    const std::string noFile = "no_file.txt";

    REQUIRE_THROWS_AS(FileHandle(noFile, "r"), ResourceError);
    const std::string file = "test.txt";
    createFile(file);
    REQUIRE_NOTHROW(FileHandle(file, "r"));
    deleteFile(file);
}

TEST_CASE("One file", "[ResourceManager]")
{
    const std::string name = "test.txt";
    createFile(name);
    ResourceManager manager;
    auto p1 = manager.getResource(name, "r");
    auto p2 = manager.getResource(name, "r");
    REQUIRE(p1.get() == p2.get());
    REQUIRE(p1.use_count() == 2);
    REQUIRE(p2.use_count() == 2);
    REQUIRE(p1->is_open());
    REQUIRE(p2->is_open());
    deleteFile(name);
}

TEST_CASE("Delete file", "[ResourseManager]")
{
    const std::string name = "test.txt";
    createFile(name);
    ResourceManager manager;
    std::weak_ptr<FileHandle> weak;
    {
        auto p1 = manager.getResource(name, "r");
        weak = p1;
        REQUIRE(p1->is_open());

        auto p2 = manager.getResource(name, "r");
        REQUIRE(p2->is_open());
        p1.reset();
        REQUIRE_FALSE(weak.expired());
        REQUIRE(p2->is_open());
    }
    REQUIRE(weak.expired());
    auto p3 = manager.getResource(name, "r");
    REQUIRE(p3->is_open());
    REQUIRE(p3.get() != nullptr);
    deleteFile(name);
}

TEST_CASE("Cashe", "[ResourceManager]")
{
    const std::string name = "test.txt";
    createFile(name);
    ResourceManager manager;
    void* address = nullptr;
    std::weak_ptr<FileHandle> weak;
    {
        auto p1 = manager.getResource(name, "r");
        address = p1.get();
        weak = p1;
    }
    auto p2 = manager.getResource(name, "r");
    REQUIRE(p2.get() != address);
    deleteFile(name);
}

TEST_CASE("Different files", "[ResourceManager]")
{
    const std::string f1 = "file1.txt";
    const std::string f2 = "file2.txt";
    createFile(f1);
    createFile(f2);
    ResourceManager manager;
    auto p1 = manager.getResource(f1, "r");
    auto p2 = manager.getResource(f2, "r");
    REQUIRE(p1.get() != p2.get());
    deleteFile(f1);
    deleteFile(f2);
}

TEST_CASE("Error", "[ResourceManager]")
{
    const std::string noFile = "no_file.txt";
    ResourceManager manager;
    REQUIRE_THROWS_AS(manager.getResource(noFile, "r"), ResourceError);
}

TEST_CASE("Closing the file", "[ResourceManager]")
{
    const std::string name = "file.txt";
    createFile(name);
    ResourceManager manager;
    auto p1 = manager.getResource(name, "r");
    REQUIRE(p1->is_open());
    p1->close();
    REQUIRE_FALSE(p1->is_open());
    auto p2 = manager.getResource(name, "r");
    REQUIRE(p2.get() == p1.get());
    REQUIRE_FALSE(p2->is_open());
    p2->open(name, "r");
    REQUIRE(p2->is_open());
    deleteFile(name);
}