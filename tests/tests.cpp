#include <catch2/catch_all.hpp>
#include <lab4/resource/ResourceError.hpp>
#include <lab4/resource/ResourceManager.hpp>
#include <lab4/resource/FileHandler.hpp>
#include <fstream>
#include <cstdio>

class TestFile
{
  public:
    TestFile(const std::string& filepath_, const std::string& content = "") : filepath{filepath_}
    {
        std::ofstream(filepath) << content;
    }

    ~TestFile()
    {
        std::remove(filepath.c_str());
    }

  private:
    std::string filepath;
};

TEST_CASE("FileHandler basic operations", "[FileHandler]")
{
    SECTION("Close file")
    {
        TestFile file{"test.txt", ""};
        lab4::resource::FileHandler handler{"test.txt"};

        handler.close();

        REQUIRE(handler.getRawPointer() == nullptr);
        REQUIRE(handler.getFilepath() == "");
    }

    SECTION("Open file after closing")
    {
        TestFile file1{"test1.txt", ""};
        TestFile file2{"test2.txt", ""};
        lab4::resource::FileHandler handler{"test1.txt"};

        handler.close();
        handler.open("test2.txt");

        REQUIRE(handler.getFilepath() == "test2.txt");
        REQUIRE(handler.getRawPointer() != nullptr);
    }

    SECTION("Open file without closing")
    {
        TestFile file1{"test1.txt", ""};
        TestFile file2{"test2.txt", ""};
        lab4::resource::FileHandler handler{"test1.txt"};

        handler.open("test2.txt");

        REQUIRE(handler.getFilepath() == "test2.txt");
        REQUIRE(handler.getRawPointer() != nullptr);
    }

    SECTION("Constructor throws on invalid file")
    {
        REQUIRE_THROWS_AS(lab4::resource::FileHandler("nonexistent.txt"), lab4::resource::ResourceError);
    }
}

TEST_CASE("FileHandler move semantics", "[FileHandler]")
{
    SECTION("Move constructor")
    {
        TestFile file{"test.txt", ""};

        lab4::resource::FileHandler handler1{"test.txt"};
        lab4::resource::FileHandler handler2{std::move(handler1)};

        REQUIRE(handler1.getRawPointer() == nullptr);
        REQUIRE(handler1.getFilepath() == "");
        REQUIRE(handler2.getRawPointer() != nullptr);
        REQUIRE(handler2.getFilepath() == "test.txt");
    }

    SECTION("Move assignment")
    {
        TestFile file{"test.txt", ""};

        lab4::resource::FileHandler handler1{"test.txt"};
        lab4::resource::FileHandler handler2 = std::move(handler1);

        REQUIRE(handler1.getRawPointer() == nullptr);
        REQUIRE(handler1.getFilepath() == "");
        REQUIRE(handler2.getRawPointer() != nullptr);
        REQUIRE(handler2.getFilepath() == "test.txt");
    }
}

TEST_CASE("ResouceManager caching", "[ResourceManager]")
{
    SECTION("ResourceManager with multiple files", "[ResourceManager]")
    {
        TestFile file1{"test1.txt", ""};
        TestFile file2{"test2.txt", ""};
        TestFile file3{"test3.txt", ""};
        lab4::resource::ResourceManager manager{};
        auto handler1 = manager.getResource("test1.txt");
        auto handler2 = manager.getResource("test2.txt");
        auto handler3 = manager.getResource("test3.txt");

        REQUIRE(manager.getDataSize() == 3);
    }

    SECTION("Same file returns same obj")
    {
        TestFile file{"test.txt", ""};
        lab4::resource::ResourceManager manager{};
        
        auto handler1 = manager.getResource("test.txt");
        auto handler2 = manager.getResource("test.txt");

        REQUIRE(handler1.get() == handler2.get());
        REQUIRE(handler1.use_count() == 2);
        REQUIRE(handler2.use_count() == 2);
    }

    SECTION("Cleanup cache")
    {
        TestFile file{"test.txt", ""};
        lab4::resource::ResourceManager manager{};

        {
            auto handler = manager.getResource("test.txt");
        }
        manager.clearExpired();

        REQUIRE(manager.getDataSize() == 0);
    }

    SECTION("Delete file from cache")
    {
        TestFile file{"test.txt", ""};
        lab4::resource::ResourceManager manager{};
        auto handler = manager.getResource("test.txt");
        
        manager.deleteResource("test.txt");

        REQUIRE(manager.getDataSize() == 0);
    }

    SECTION("Deleting invalid file does nothing")
    {
        TestFile file{"test.txt", ""};
        lab4::resource::ResourceManager manager{};
        auto handler = manager.getResource("test.txt");

        REQUIRE_THROWS_AS(manager.deleteResource("nonexistent.txt"), lab4::resource::ResourceError);
    }

    SECTION("ResourceManager throws on invalid file")
    {
        lab4::resource::ResourceManager manager{};
        REQUIRE_THROWS_AS(manager.getResource("nonexistent.txt"), lab4::resource::ResourceError);
    }
}