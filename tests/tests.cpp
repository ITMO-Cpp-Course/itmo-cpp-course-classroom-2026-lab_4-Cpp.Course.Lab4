#include <catch2/catch_all.hpp>
#include <fcntl.h>
#include <lab4/resource/resource_FileHandle.hpp>
#include <lab4/resource/resource_ResourceError.hpp>
#include <lab4/resource/resource_ResourceManager.hpp>
using namespace lab4::resource;
TEST_CASE("Test Resources")
{
    ResourceManager manager;
    auto handle1 = manager.acquire("test_data.txt", O_RDONLY);
    auto handle2 = manager.acquire("test_data.txt", O_RDONLY);

    char buffer[128];
    long n = handle1->read(buffer, sizeof(buffer));
    REQUIRE(n == 16);
    REQUIRE(buffer[7] == ' ');
    REQUIRE(buffer[8] == '1');
    REQUIRE((*handle1) == (*handle2));
}
TEST_CASE("Test Exception")
{
    ResourceManager manager;
    try
    {
        auto handle1 = manager.acquire("not existent.txt", O_RDONLY);
        char buffer[128];
        handle1->read(buffer, sizeof(buffer));
        REQUIRE(false);
    }
    catch (const ResourceError& e)
    {
        REQUIRE(true);
    }
}
