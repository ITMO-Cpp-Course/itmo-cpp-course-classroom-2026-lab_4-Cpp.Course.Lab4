cmake_minimum_required(VERSION 3.20)
project(lab4_resource_core VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_TESTS "Build unit tests" ON)

add_subdirectory(src/resource_core)

if (BUILD_TESTS)
    enable_testing()

    include(FetchContent)
    FetchContent_Declare(
            Catch2
            GIT_REPOSITORY https://github.com/catchorg/Catch2.git
            GIT_TAG v3.5.2
    )
    FetchContent_MakeAvailable(Catch2)

    add_executable(resource_core_tests
            tests/tests.cpp
            src/resource_core/ResourceError.cpp
            include/resource_core/FileHandle.hpp
            include/resource_core/ResourceManager.hpp
    )

    target_link_libraries(resource_core_tests
            PRIVATE
            resource_core
            Catch2::Catch2WithMain
    )

    target_include_directories(resource_core_tests PRIVATE include)

    include(CTest)
    include(Catch)
    catch_discover_tests(resource_core_tests)
endif ()