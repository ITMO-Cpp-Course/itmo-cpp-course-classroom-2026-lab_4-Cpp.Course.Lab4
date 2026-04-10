#include "resource_core/FileHandle.hpp"
#include "resource_core/ResourceError.hpp"
#include "resource_core/ResourceManager.hpp"
#include <catch2/catch_all.hpp>
#include <fstream>

using namespace lab4::resource;

void make_file(const std::string& name, const std::string& content)
{
    std::ofstream file(name); // файловый поток для записи
    file << content;          // file - поток
}

void delete_file(const std::string& name)
{
    std::remove(name.c_str());
}

TEST_CASE("ResourceError - получение исключения", "[ResourceError]")
{
    try
    {
        throw ResourceError("Ошибка!");
    }
    catch (const ResourceError& e)
    {
        REQUIRE(true);
    }
}

TEST_CASE("FileHandle - можно создать пустой", "[FileHandle]")
{
    FileHandle fh;
    REQUIRE(fh.is_open() == false);
}

TEST_CASE("FileHandle - можно открыть файл", "[FileHandle]")
{
    make_file("test.txt", "hello");

    FileHandle fh("test.txt", "r");
    REQUIRE(fh.is_open() == true);

    delete_file("test.txt");
}

TEST_CASE("FileHandle - можно прочитать строку", "[FileHandle]")
{
    make_file("test.txt", "Hello\nWorld");

    FileHandle fh("test.txt", "r");
    REQUIRE(fh.read_line() == "Hello");
    REQUIRE(fh.read_line() == "World");

    delete_file("test.txt");
}

TEST_CASE("FileHandle - можно записать строку", "[FileHandle]")
{
    FileHandle fh("output.txt", "w");
    fh.write_line("Hello");
    fh.close();

    std::ifstream file("output.txt");
    std::string line;
    std::getline(file, line);
    REQUIRE(line == "Hello");

    delete_file("output.txt");
}

TEST_CASE("FileHandle - можно закрыть файл", "[FileHandle]")
{
    make_file("test.txt", "hello");

    FileHandle fh("test.txt", "r");
    REQUIRE(fh.is_open() == true);

    fh.close();
    REQUIRE(fh.is_open() == false);

    delete_file("test.txt");
}

TEST_CASE("FileHandle - можно переместить файл", "[FileHandle]")
{
    make_file("test.txt", "data");

    FileHandle fh1("test.txt", "r");
    FileHandle fh2 = std::move(fh1);

    REQUIRE(fh1.is_open() == false);
    REQUIRE(fh2.is_open() == true);

    delete_file("test.txt");
}

TEST_CASE("ResourceManager - можно получить файл", "[ResourceManager]")
{
    make_file("test.txt", "data");

    ResourceManager rm;
    auto file = rm.get_file("test.txt", "r");

    REQUIRE(file->is_open() == true);

    delete_file("test.txt");
}

TEST_CASE("ResourceManager - повторный запрос дает тот же файл", "[ResourceManager]")
{
    make_file("test.txt", "data");

    ResourceManager rm;
    auto file1 = rm.get_file("test.txt", "r");
    auto file2 = rm.get_file("test.txt", "r");

    REQUIRE(file1.get() == file2.get());

    delete_file("test.txt");
}

TEST_CASE("ResourceManager - можно проверить есть ли файл в кеше", "[ResourceManager]")
{
    make_file("test.txt", "data");

    ResourceManager rm;

    REQUIRE(rm.is_cached("test.txt") == false); // нет

    auto file = rm.get_file("test.txt", "r"); // получили

    REQUIRE(rm.is_cached("test.txt") == true); // да

    delete_file("test.txt");
}

TEST_CASE("ResourceManager - очистка удаляет мертвые файлы", "[ResourceManager]")
{
    make_file("test.txt", "data");

    ResourceManager rm;

    {
        auto file = rm.get_file("test.txt", "r");
        REQUIRE(rm.is_cached("test.txt") == true);
    } // файл уничтожился

    rm.cleanup();

    REQUIRE(rm.is_cached("test.txt") == false);

    delete_file("test.txt");
}

TEST_CASE("запись и чтение через менеджер", "[Integration]")
{
    ResourceManager rm;

    {
        auto file = rm.get_file("test.txt", "w");
        file->write_line("Привет");
        file->write_line("Мир");
    }

    {
        auto file = rm.get_file("test.txt", "r");
        REQUIRE(file->read_line() == "Привет");
        REQUIRE(file->read_line() == "Мир");
    }

    delete_file("test.txt");
}
