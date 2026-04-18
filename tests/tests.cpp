#include "lab4_hpp/file_handle.hpp"
#include "lab4_hpp/resource_error.hpp"
#include "lab4_hpp/resource_manager.hpp"
#include <catch2/catch_all.hpp>
#include <fstream>
#include <iostream>
#include <thread>
#include <vector>
#include <cassert>

using namespace lab4::resource;

using namespace Catch::Matchers;

TEST_CASE("FileHandle constructor", "[FileHandle]")
{
    SECTION("Successfully opens file for writing")
    {
        FileHandle file("test1.txt", std::ios::out);
        REQUIRE(file.IsOpen() == true);
        REQUIRE(file.GetFileName() == "test1.txt");
        std::remove("test1.txt");
    }

    SECTION("Уxception when opening non-existent file")
    {
        REQUIRE_THROWS_AS(FileHandle("nonexistent_dir/test.txt", std::ios::in), ResourceError);
    }
}

TEST_CASE("FileHandle write and read", "[FileHandle]")
{
    SECTION("Write then read a line")
    {
        // Запись
        {
            FileHandle write_file("test2.txt", std::ios::out);
            write_file.WriteLine("GOOD");
            write_file.WriteLine("MORNING");
        }

        // Чтение
        {
            FileHandle read_file("test2.txt", std::ios::in);
            REQUIRE(read_file.ReadLine() == "GOOD");
            REQUIRE(read_file.ReadLine() == "MORNING");
        }

        std::remove("test2.txt");
    }

    SECTION("Write then read all data")
    {
        // Создаём файл через обычный поток
        {
            std::ofstream f("test3.txt");
            f << "Line1\nLine2\nLine3";
        }

        {
            FileHandle file("test3.txt", std::ios::in);
            std::string content = file.ReadAll();
            REQUIRE(data.find("Line1") != std::string::npos);
            REQUIRE(data.find("Line2") != std::string::npos);
            REQUIRE(data.find("Line3") != std::string::npos);
        }

        std::remove("test3.txt");
    }
}

TEST_CASE("FileHandle move ", "[FileHandle]")
{
    {
        FileHandle f1("test4.txt", std::ios::out);
        f1.WriteLine("Original");

        FileHandle f2 = std::move(f1);

        REQUIRE(f1.IsOpen() == false); // f1 больше не владеет
        REQUIRE(f2.IsOpen() == true);  // f2 владеет
    }

    std::remove("test4.txt");
}

TEST_CASE("ResourceManager clear removes all", "[ResourceManager]")
{
    auto& manager = ResourceManager::GetObject();
    manager.ClearData();

    manager.GetFile("clear1.txt", std::ios::out);
    manager.GetFile("clear2.txt", std::ios::out);
    manager.GetFile("clear3.txt", std::ios::out);

    REQUIRE(manager.DataSize() == 3);

    manager.ClearData();

    REQUIRE(manager.DataSize() == 0);
    REQUIRE(manager.IsData("clear1.txt") == false);

    std::remove("clear1.txt");
    std::remove("clear2.txt");
    std::remove("clear3.txt");
}

TEST_CASE("FileHandle and ResourceManager ", "[Integration]")
{
    auto& manager = ResourceManager::GetObject();
    manager.ClearData();

    SECTION("Write through one pointer, read through another")
    {
        auto writer = manager.GetFile("shared.txt", std::ios::out);
        writer->WriteLine("Shared data");
        writer->WriteLine("More data");

        auto reader = manager.GetFile("shared.txt", std::ios::in);
        reader->seek_to_begin();

        REQUIRE(reader->ReadLine() == "Shared data");
        REQUIRE(reader->ReadLine() == "More data");
    }

    SECTION("Data persists in cache")
    {
        std::string content;

        {
            auto f1 = manager.GetFile("persist.txt", std::ios::out);
            f1->WriteLine("Persistent content");
        } // f1 уничтожается

        // Объект удалился, weak_ptr expired
        REQUIRE(manager.IsData("persist.txt") == false);

        // Создаём заново
        auto f2 = manager.GetFile("persist.txt", std::ios::out);
        f2->WriteLine("New content");

        // Проверяем
        auto f3 = manager.GetFile("persist.txt", std::ios::in);
        REQUIRE(f3->ReadLine() == "New content");
    }

    manager.ClearData();
    std::remove("shared.txt");
    std::remove("persist.txt");
}