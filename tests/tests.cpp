#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include "resource_core/resource_core.hpp"
#include <fstream>
#include <cstdio>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

using namespace lab4::resource;

// Вспомогательная функция: создать временный файл с содержимым
static std::string create_temp_file(const std::string& name, const std::string& content = "") {
    std::string path = "/tmp/" + name;
    std::ofstream ofs(path);
    ofs << content;
    ofs.close();
    return path;
}

// Вспомогательная функция: удалить временный файл
static void remove_temp_file(const std::string& path) {
    std::remove(path.c_str());
}


// ТЕСТЫ: ResourceError

TEST_CASE("ResourceError contains message and error code", "[ResourceError]") {
    SECTION("Error with system code stores errno") {
        ResourceError err("Failed to open file", ENOENT); // 2 = No such file
        REQUIRE(err.code() == ENOENT);
        REQUIRE_THAT(err.what(), Catch::Matchers::ContainsSubstring("Failed to open file"));
        REQUIRE_THAT(err.what(), Catch::Matchers::ContainsSubstring("No such file"));
    }

    SECTION("Error without system code has code 0") {
        ResourceError err("Custom error");
        REQUIRE(err.code() == 0);
        REQUIRE_THAT(err.what(), Catch::Matchers::ContainsSubstring("Custom error"));
    }
}


// ТЕСТЫ: FileHandle — открытие и закрытие

TEST_CASE("FileHandle opens and closes files", "[FileHandle]") {
    const std::string path = "/tmp/test_open_close.txt";

    // Очистка перед тестом
    std::remove(path.c_str());

    SECTION("Constructor opens file for reading (creates if needed)") {
        // Создаём файл вручную
        {
            std::ofstream ofs(path);
            ofs << "data";
        }

        FileHandle fh(path, "r");
        REQUIRE(fh.is_open());
        REQUIRE(fh.native_handle() >= 0);
        REQUIRE(fh.path() == path);
    }

    SECTION("Constructor opens file for writing (creates new)") {
        FileHandle fh(path, "w");
        REQUIRE(fh.is_open());
        REQUIRE(fh.native_handle() >= 0);
    }

    SECTION("Explicit close makes handle not open") {
        FileHandle fh(path, "w");
        REQUIRE(fh.is_open());
        fh.close();
        REQUIRE_FALSE(fh.is_open());
    }

    SECTION("Destructor closes file automatically") {
        {
            FileHandle fh(path, "w");
            REQUIRE(fh.is_open());
        }
        // После выхода из блока файл должен быть закрыт
        // Проверяем: можем снова открыть (дескриптор не занят)
        REQUIRE_NOTHROW(FileHandle(path, "r"));
    }

    SECTION("Opening non-existent file for reading throws") {
        std::string nonexistent = "/tmp/nonexistent_file_12345.txt";
        REQUIRE_THROWS_AS(FileHandle(nonexistent, "r"), ResourceError);
    }

    SECTION("Unsupported mode throws ResourceError") {
        REQUIRE_THROWS_AS(FileHandle(path, "x"), ResourceError);
    }

    std::remove(path.c_str());
}


// ТЕСТЫ: FileHandle — режимы открытия


TEST_CASE("FileHandle supports all open modes", "[FileHandle]") {
    const std::string path = "/tmp/test_modes.txt";

    SECTION("Mode 'r' — read only") {
        {
            std::ofstream ofs(path);
            ofs << "hello";
        }
        FileHandle fh(path, "r");
        REQUIRE(fh.is_open());
    }

    SECTION("Mode 'r+' — read/write") {
        {
            std::ofstream ofs(path);
            ofs << "hello";
        }
        FileHandle fh(path, "r+");
        REQUIRE(fh.is_open());
    }

    SECTION("Mode 'w' — write only, create/truncate") {
        FileHandle fh(path, "w");
        REQUIRE(fh.is_open());
    }

    SECTION("Mode 'w+' — read/write, create/truncate") {
        FileHandle fh(path, "w+");
        REQUIRE(fh.is_open());
    }

    SECTION("Mode 'a' — append only, create") {
        FileHandle fh(path, "a");
        REQUIRE(fh.is_open());
    }

    SECTION("Mode 'a+' — append/read, create") {
        FileHandle fh(path, "a+");
        REQUIRE(fh.is_open());
    }

    std::remove(path.c_str());
}


// ТЕСТЫ: FileHandle — move-семантика


TEST_CASE("FileHandle move semantics", "[FileHandle]") {
    const std::string path = "/tmp/test_move.txt";
    std::remove(path.c_str());

    SECTION("Move constructor transfers ownership") {
        FileHandle fh1(path, "w");
        int fd1 = fh1.native_handle();
        REQUIRE(fh1.is_open());

        FileHandle fh2(std::move(fh1));

        REQUIRE_FALSE(fh1.is_open());      // источник обнулён
        REQUIRE(fh2.is_open());            // приёмник владеет
        REQUIRE(fh2.native_handle() == fd1); // тот же дескриптор
    }

    SECTION("Move assignment transfers ownership and closes previous") {
        FileHandle fh1(path, "w");
        int fd1 = fh1.native_handle();

        std::string path2 = "/tmp/test_move2.txt";
        std::remove(path2.c_str());
        FileHandle fh2(path2, "w");
        int fd2 = fh2.native_handle();
        REQUIRE(fd1 != fd2); // разные дескрипторы

        fh2 = std::move(fh1);

        REQUIRE_FALSE(fh1.is_open());      // источник обнулён
        REQUIRE(fh2.is_open());            // приёмник владеет новым
        REQUIRE(fh2.native_handle() == fd1); // теперь fd1
        // fd2 должен был закрыться при присваивании
    }

    SECTION("Default constructed handle can be move-assigned") {
        FileHandle fh1(path, "w");
        int fd = fh1.native_handle();

        FileHandle fh2; // не открыт
        REQUIRE_FALSE(fh2.is_open());

        fh2 = std::move(fh1);

        REQUIRE_FALSE(fh1.is_open());
        REQUIRE(fh2.is_open());
        REQUIRE(fh2.native_handle() == fd);
    }

    std::remove(path.c_str());
    std::remove("/tmp/test_move2.txt");
}


// ТЕСТЫ: FileIO — чтение и запись


TEST_CASE("FileIO read and write operations", "[FileIO]") {
    const std::string path = "/tmp/test_io.txt";

    SECTION("write_all writes complete string") {
        std::remove(path.c_str());
        FileHandle fh(path, "w");
        write_all(fh, "Hello, world!");
        fh.close();

        // Проверяем через стандартный ifstream
        std::ifstream ifs(path);
        std::string content;
        std::getline(ifs, content);
        REQUIRE(content == "Hello, world!");
    }

    SECTION("write_all to closed file throws") {
        FileHandle fh(path, "w");
        fh.close();
        REQUIRE_THROWS_AS(write_all(fh, "data"), ResourceError);
    }

    SECTION("read_all reads entire file") {
        std::remove(path.c_str());
        {
            std::ofstream ofs(path);
            ofs << "Line 1\nLine 2\nLine 3";
        }

        FileHandle fh(path, "r");
        std::string content = read_all(fh);
        REQUIRE(content == "Line 1\nLine 2\nLine 3");
    }

    SECTION("read_all from closed file throws") {
        FileHandle fh(path, "w");
        fh.close();
        REQUIRE_THROWS_AS(read_all(fh), ResourceError);
    }

    SECTION("read_all returns empty string for empty file") {
        std::remove(path.c_str());
        {
            std::ofstream ofs(path); // пустой файл
        }

        FileHandle fh(path, "r");
        std::string content = read_all(fh);
        REQUIRE(content.empty());
    }

    std::remove(path.c_str());
}


// ТЕСТЫ: ResourceManager — базовое кеширование


TEST_CASE("ResourceManager caches file handles", "[ResourceManager]") {
    const std::string path = "/tmp/test_cache.txt";
    std::remove(path.c_str());

    ResourceManager mgr;

    SECTION("acquire returns shared_ptr to FileHandle") {
        auto handle = mgr.acquire(path, "w");
        REQUIRE(handle != nullptr);
        REQUIRE(handle->is_open());
        REQUIRE(handle->path() == path);
    }

    SECTION("Repeated acquire returns same handle") {
        auto h1 = mgr.acquire(path, "w");
        auto h2 = mgr.acquire(path, "w");

        REQUIRE(h1 == h2); // тот же shared_ptr
        REQUIRE(h1->native_handle() == h2->native_handle()); // тот же fd
    }

    SECTION("Acquire with different mode throws") {
        auto h1 = mgr.acquire(path, "r"); // открыт на чтение
        REQUIRE_THROWS_AS(mgr.acquire(path, "w"), ResourceError); // запросили запись
    }

    std::remove(path.c_str());
}


// ТЕСТЫ: ResourceManager — разделяемое владение


TEST_CASE("ResourceManager shared ownership", "[ResourceManager]") {
    const std::string path = "/tmp/test_shared.txt";
    std::remove(path.c_str());

    ResourceManager mgr;

    SECTION("File stays open while any shared_ptr exists") {
        auto h1 = mgr.acquire(path, "w");
        int fd = h1->native_handle();

        {
            auto h2 = mgr.acquire(path, "w");
            REQUIRE(h2->native_handle() == fd);
        } // h2 уничтожен, но h1 ещё жив

        // Файл должен быть ещё открыт
        REQUIRE(h1->is_open());
    }

    SECTION("File closes when last shared_ptr is destroyed") {
        int fd;
        {
            auto h1 = mgr.acquire(path, "w");
            fd = h1->native_handle();
        }
        // h1 уничтожен — файл должен быть закрыт

        // Проверяем: открываем заново, дескриптор может быть другим
        auto h2 = mgr.acquire(path, "r");
        REQUIRE(h2->native_handle() != fd); // новый дескриптор
    }

    std::remove(path.c_str());
}


// ТЕСТЫ: ResourceManager — управление кешем


TEST_CASE("ResourceManager cache management", "[ResourceManager]") {
    const std::string path1 = "/tmp/test_cache1.txt";
    const std::string path2 = "/tmp/test_cache2.txt";
    std::remove(path1.c_str());
    std::remove(path2.c_str());

    ResourceManager mgr;

    SECTION("cache_size reflects number of entries") {
        REQUIRE(mgr.cache_size() == 0);

        auto h1 = mgr.acquire(path1, "w");
        REQUIRE(mgr.cache_size() == 1);

        auto h2 = mgr.acquire(path2, "w");
        REQUIRE(mgr.cache_size() == 2);

        // тот же path1 — размер не меняется
        auto h3 = mgr.acquire(path1, "w");
        REQUIRE(mgr.cache_size() == 2);
    }

    SECTION("purge_expired removes dead entries") {
        {
            auto h1 = mgr.acquire(path1, "w");
            REQUIRE(mgr.cache_size() == 1);
        }

        mgr.purge_expired();
        REQUIRE(mgr.cache_size() == 0);
    }

    SECTION("release forces file closure and cache removal") {
        auto h1 = mgr.acquire(path1, "w");
        REQUIRE(mgr.cache_size() == 1);

        bool removed = mgr.release(path1);
        REQUIRE(removed);
        REQUIRE(mgr.cache_size() == 0);
        REQUIRE_FALSE(h1->is_open()); // дескриптор закрыт
    }

    SECTION("release non-existent path returns false") {
        bool removed = mgr.release("/tmp/nonexistent.txt");
        REQUIRE_FALSE(removed);
    }

    SECTION("contains checks if live handle exists") {
        auto h1 = mgr.acquire(path1, "w");
        REQUIRE(mgr.contains(path1));

        mgr.release(path1);
        REQUIRE_FALSE(mgr.contains(path1));
    }

    std::remove(path1.c_str());
    std::remove(path2.c_str());
}


// ТЕСТЫ: Интеграционный сценарий использования

TEST_CASE("Integration: full workflow with ResourceManager", "[Integration]") {
    const std::string path = "/tmp/test_integration.txt";
    std::remove(path.c_str());

    ResourceManager mgr;

    SECTION("Write, close, read back via different handles") {
        // Запись
        {
            auto h = mgr.acquire(path, "w");
            write_all(*h, "Integration test data");
        }

        // Чтение (должен открыться заново, т.к. прошлый закрыт)
        {
            auto h = mgr.acquire(path, "r");
            std::string content = read_all(*h);
            REQUIRE(content == "Integration test data");
        }
    }






    SECTION("Two concurrent readers share same handle") {
        // Создаём файл
        {
            auto h = mgr.acquire(path, "w");
            write_all(*h, "Shared read test");
        }

        // Два читателя
        auto reader1 = mgr.acquire(path, "r");
        auto reader2 = mgr.acquire(path, "r");

        REQUIRE(reader1 == reader2); // тот же объект

        read_all(*reader1); // не должно мешать reader2
        std::string content = read_all(*reader2);
        REQUIRE(content == "Shared read test");
    }




    std::remove(path.c_str());
}


// ТЕСТЫ: Граничные случаи


TEST_CASE("Edge cases", "[EdgeCases]") {
    SECTION("Double close on FileHandle is safe") {
        const std::string path = "/tmp/test_double_close.txt";
        std::remove(path.c_str());

        FileHandle fh(path, "w");
        fh.close();
        REQUIRE_NOTHROW(fh.close()); // второй раз не бросает
    }

    SECTION("Moving from closed handle") {
        const std::string path = "/tmp/test_move_closed.txt";
        std::remove(path.c_str());

        FileHandle fh1(path, "w");
        fh1.close();
        REQUIRE_FALSE(fh1.is_open());

        FileHandle fh2(std::move(fh1));
        REQUIRE_FALSE(fh2.is_open()); // переместили закрытый
    }

    SECTION("write_all and read_all with empty string") {
        const std::string path = "/tmp/test_empty_string.txt";
        std::remove(path.c_str());

        FileHandle fh(path, "w");
        REQUIRE_NOTHROW(write_all(fh, "")); // пустая строка
        fh.close();

        FileHandle fh2(path, "r");
        std::string content = read_all(fh2);
        REQUIRE(content.empty());
    }

    SECTION("FileHandle default constructor creates invalid handle") {
        FileHandle fh;
        REQUIRE_FALSE(fh.is_open());
        REQUIRE(fh.native_handle() == -1);
    }

    std::remove("/tmp/test_double_close.txt");
    std::remove("/tmp/test_move_closed.txt");
    std::remove("/tmp/test_empty_string.txt");
}

//jyfjy



//hthtr