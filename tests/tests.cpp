#include "ResourceManager.hpp"
#include "resource_error.hpp"
#include <catch2/catch_all.hpp>
#include <fstream>

using namespace lab4::resource;

// Вспомогательная функция для создания файла перед тестом
void prepare_file(const std::string& name)
{
    std::ofstream f(name);
    f << "data";
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

        CHECK(res1.use_count() >= 3);
    }

    SECTION("Error handling for missing files")
    {
        REQUIRE_THROWS_AS(manager.get_resource("ghost.txt"), ResourceError);
    }

    SECTION("Garbage collection")
    {
        auto res = manager.get_resource(filename);
        long initial_count = res.use_count();

        manager.collect_garbage();

        CHECK(res.use_count() == initial_count - 1);
    }
}