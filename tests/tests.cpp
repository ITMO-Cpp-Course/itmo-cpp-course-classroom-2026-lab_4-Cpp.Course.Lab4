#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>

#include "lab4/resource.hpp"
#include <cstdio>
#include <fstream>

using namespace lab4::resource;

static void create_test_file(const std::string& name, const std::string& content)
{
    FILE* f = std::fopen(name.c_str(), "w");
    REQUIRE(f != nullptr);
    std::fprintf(f, "%s", content.c_str());
    std::fclose(f);
}

static void remove_test_file(const std::string& name)
{
    std::remove(name.c_str());
}

TEST_CASE("FileHandle automatically closes file on destruction", "[RAII]")
{
    const std::string filename = "raii_test.txt";
    create_test_file(filename, "test");

    {
        FileHandle fh(filename);
        REQUIRE(fh.is_open());
    }

    std::ifstream test(filename);
    REQUIRE(test.is_open());
    test.close();

    remove_test_file(filename);
}

TEST_CASE("FileHandle writes data using mode \"w\"", "[FileHandle]")
{
    const std::string filename = "write_test.txt";

    {
        FileHandle fw(filename, "w");
        fw.write_line("Hello, world!");
        fw.write_line("Second line");
    }

    std::ifstream in(filename);
    REQUIRE(in.is_open());
    std::string line1, line2;
    std::getline(in, line1);
    std::getline(in, line2);
    REQUIRE(line1 == "Hello, world!");
    REQUIRE(line2 == "Second line");
    in.close();

    remove_test_file(filename);
}

TEST_CASE("FileHandle reads lines correctly", "[FileHandle]")
{
    const std::string filename = "read_test.txt";
    create_test_file(filename, "First line\nSecond line\n");

    FileHandle fr(filename, "r");
    REQUIRE(fr.is_open());

    std::string l1 = fr.read_line();
    std::string l2 = fr.read_line();
    std::string l3 = fr.read_line();

    REQUIRE(l1 == "First line");
    REQUIRE(l2 == "Second line");
    REQUIRE(l3 == "");

    remove_test_file(filename);
}

TEST_CASE("ResourceManager shares ownership of the same file", "[ResourceManager]")
{
    const std::string filename = "shared_test.txt";
    create_test_file(filename, "content");

    ResourceManager rm;
    auto p1 = rm.get(filename);
    auto p2 = rm.get(filename);

    REQUIRE(p1.get() == p2.get());
    REQUIRE(p1.use_count() >= 2);

    p1.reset();
    REQUIRE(p2.use_count() == 1);

    remove_test_file(filename);
}

TEST_CASE("Cache expires when last shared_ptr is destroyed", "[ResourceManager]")
{
    const std::string filename = "cache_expire.txt";
    create_test_file(filename, "Version 1");

    ResourceManager rm;
    std::shared_ptr<FileHandle> keeper;
    {
        auto p = rm.get(filename);
        keeper = p;
    }

    auto p2 = rm.get(filename);
    REQUIRE(p2.get() == keeper.get());

    keeper.reset();

    create_test_file(filename, "Version 2");

    auto p3 = rm.get(filename);
    std::string line = p3->read_line();
    REQUIRE(line == "Version 2");

    remove_test_file(filename);
}

TEST_CASE("Opening non-existent file throws ResourceError", "[ResourceError]")
{
    ResourceManager rm;
    REQUIRE_THROWS_AS(rm.get("definitely_not_existing_xyz.txt"), ResourceError);
}

TEST_CASE("Write to read-only file throws ResourceError", "[ResourceError]")
{
    const std::string filename = "readonly.txt";
    create_test_file(filename, "some data");

    FileHandle fr(filename, "r");
    REQUIRE(fr.is_open());
    REQUIRE_THROWS_AS(fr.write_line("Should fail"), ResourceError);

    remove_test_file(filename);
}

TEST_CASE("FileHandle move transfers ownership", "[FileHandle]")
{
    const std::string filename = "move_test.txt";
    create_test_file(filename, "Move test");

    FileHandle fh1(filename);
    REQUIRE(fh1.is_open());

    FileHandle fh2 = std::move(fh1);
    REQUIRE_FALSE(fh1.is_open());
    REQUIRE(fh2.is_open());

    std::string line = fh2.read_line();
    REQUIRE(line == "Move test");

    remove_test_file(filename);
}