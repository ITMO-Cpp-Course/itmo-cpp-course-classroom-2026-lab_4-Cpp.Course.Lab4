#include "FileHandle.hpp"
#include "ResourceError.hpp"
#include "ResourceManager.hpp"
#include <catch2/catch_all.hpp>
#include <fstream>

using namespace lab4::resource;

// Вспомогательная функция для создания файла перед тестом
void prepare_file(const std::string& name)
{
    std::ofstream f(name);
    f << "data";
}

TEST_CASE("FileHandle technical requirements", "[file_handle]")
{
    const std::string filename = "handle_test.txt";
    prepare_file(filename);

    SECTION("RAII semantics")
    {
        {
            FileHandle fh(filename, "r");
            REQUIRE(fh.ready_for_use());
        }
    }

    SECTION("Move semantics")
    {
        FileHandle fh1(filename, "r");
        void* addr = fh1.get();
        FileHandle fh2 = std::move(fh1);

        CHECK_FALSE(fh1.ready_for_use());
        CHECK(fh2.ready_for_use());
        CHECK(fh2.get() == addr);
    }

    SECTION("Invalid file handling")
    {
        REQUIRE_THROWS_AS(FileHandle("non_existent.txt", "r"), ResourceError);
    }
}

TEST_CASE("ResourceManager basic logic", "[resource_manager]")
{
    ResourceManager manager;
    const std::string filename = "test_file.txt";
    prepare_file(filename);

    SECTION("Resource acquisition and caching")
    {
        auto res1 = manager.get_resource(filename);

        REQUIRE(res1 != nullptr);

        auto res2 = manager.get_resource(filename);
        CHECK(res1.get() == res2.get());

        CHECK(res1.use_count() >= 2);
    }

    SECTION("Error handling for missing files")
    {
        REQUIRE_THROWS_AS(manager.get_resource("ghost.txt"), ResourceError);
    }

    SECTION("Clear everything")
    {
        auto res = manager.get_resource(filename);
        long initial_count = res.use_count();

        manager.clear_everything();

        CHECK(res.use_count() == initial_count - 1);
    }
}
