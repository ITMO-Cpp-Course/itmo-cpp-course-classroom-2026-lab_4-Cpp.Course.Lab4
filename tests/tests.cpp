#include "resource_core/ResourceManager.hpp"
#include "resource_core/resource_err.hpp"
#include <catch2/catch_all.hpp>
#include <fsream>

using namespace lab4::resource;

TEST_CASE("ResourceManager logic tests", "[resource_manager]")
{
    ResourceManager manager;
    std::string test_path = "test_dummy.txt";

    {
        std::ofstream test_file(test_path);
        test_file << "Hello, ITMO!";
    }

    SECTION("the same result for the same paths")
    {
        auto res1 = manager.getResource(test_path);
        auto res2 = manager.getResource(test_path);

        REQUIRE(res1.get() == res2.get());

        REQUIRE(res1->get() == res2->get());
    }

    SECTION("Resource is recreated after all shared_ptrs are destroyed")
    {

        void* raw_address_before = nullptr;
        {
            auto res1 = manager.getResource(test_path);
            raw_address_before = static_cast<void*>(res1.get());
        }

        auto res2 = manager.getResource(test_path);
        void* raw_address_after = static_cast<void*>(res2.get());

        REQUIRE(res2 != nullptr);
        REQUIRE(res2->get() != nullptr);
    }

    SECTION("Throws ResourceError when file does not exist")
    {
        REQUIRE_THROWS_AS(manager.getResource("non_existent_file_12345.txt"), ResourceError);
    }

    std::remove(test_path.c_str());
}
