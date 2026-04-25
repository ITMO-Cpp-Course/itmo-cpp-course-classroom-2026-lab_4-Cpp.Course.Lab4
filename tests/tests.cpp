#include <catch2/catch_all.hpp>
#include <filesystem>
#include <fstream>
#include <memory>

#include "lab4/resource/FileHandle.hpp"
#include "lab4/resource/ResourceError.hpp"
#include "lab4/resource/ResourceManager.hpp"

using namespace lab4::resource;

void create_temp_file(const std::string& path)
{
    std::ofstream f(path);
    f << "test data";
    f.close();
}

TEST_CASE("ResourceManager: Comprehensive Testing", "[resource][manager]")
{
    ResourceManager rm;
    const std::string test_file = "test_resource.txt";
    const std::string second_file = "another_resource.txt";

    create_temp_file(test_file);
    create_temp_file(second_file);

    SECTION("Basic Loading and Caching")
    {
        auto res1 = rm.getResource(test_file);
        auto res2 = rm.getResource(test_file);

        REQUIRE(res1.get() == res2.get());

        REQUIRE(res1->isOpen() == true);

        REQUIRE(rm.getResourceCount() == 1);
    }

    SECTION("Independent Resources")
    {
        auto res1 = rm.getResource(test_file);
        auto res2 = rm.getResource(second_file);

        REQUIRE(res1.get() != res2.get());
        REQUIRE(rm.getResourceCount() == 2);
    }

    SECTION("RAII")
    {
        {
            auto temporary_res = rm.getResource(test_file);
            REQUIRE(rm.getResourceCount() == 1);
        }
        REQUIRE(rm.getResourceCount() == 0);
    }

    SECTION("Error: Missing Files")
    {
        REQUIRE_THROWS_AS(rm.getResource("void.txt"), ResourceError);
    }
    std::filesystem::remove(test_file);
    std::filesystem::remove(second_file);
}
