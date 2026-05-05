#include "resource_core/resource_error.hpp"
#include "resource_core/resource_manager.hpp"
#include <catch2/catch_all.hpp>

using namespace lab4::resource;

TEST_CASE("Проверка работы кеша и разделяемого использования", "[ResourceManager]")
{
    ResourceManager manager;

    std::shared_ptr<FileHandle> file1 = manager.GetFile("test.txt", "w");
    std::shared_ptr<FileHandle> file2 = manager.GetFile("test.txt", "w");

    REQUIRE(file1 == file2);
    REQUIRE(manager.IsOpened("test.txt") == true);
}

TEST_CASE("Проверка времени жизни (RAII)", "[ResourceManager]")
{
    ResourceManager manager;

    std::shared_ptr<FileHandle> file1 = manager.GetFile("test.txt", "w");
    std::shared_ptr<FileHandle> file2 = manager.GetFile("test.txt", "w");

    file1.reset();
    REQUIRE(manager.IsOpened("test.txt") == true);

    file2.reset();
    REQUIRE(manager.IsOpened("test.txt") == false);
}

TEST_CASE("Проверка обработки ошибок", "[ResourceManager]")
{
    ResourceManager manager;

    REQUIRE_THROWS_AS(manager.GetFile("invalid_directory/non_existent.txt", "r"), ResourceError);
}