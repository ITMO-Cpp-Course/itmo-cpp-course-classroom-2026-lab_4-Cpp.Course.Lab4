cmake_minimum_required(VERSION 3.20)

project(Lab4 LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options
option(STRICT_WARNINGS "Enable extra warnings and treat warnings as errors" ON)
option(ENABLE_SANITIZERS "Enable AddressSanitizer" ON)

# Common warnings
add_library(project_warnings INTERFACE
        tests/tests.cpp
        tests/tests.cpp
        tests/tests.cpp
        tests/tests.cpp)
if (MSVC)
    target_compile_options(project_warnings INTERFACE
            /W4 /permissive- /Zc:preprocessor /Zc:__cplusplus /utf-8
            $<$<BOOL:${STRICT_WARNINGS}>:/WX>
    )
else ()
    target_compile_options(project_warnings INTERFACE
            -Wall -Wextra -Wpedantic
            -Wconversion -Wsign-conversion -Wshadow
            -Wnon-virtual-dtor -Wold-style-cast -Woverloaded-virtual
            -Wnull-dereference -Wdouble-promotion -Wformat=2
            $<$<BOOL:${STRICT_WARNINGS}>:-Werror>
    )
endif ()

include(CTest)
enable_testing()

# AddressSanitizer (ASan) as an interface target
# Sanitizers are not supported on Windows (neither MSVC nor MinGW)
if (ENABLE_SANITIZERS AND NOT WIN32)
    add_library(project_sanitizers INTERFACE
            include/resource_error.hpp
            include/file_handle.hpp
            include/resource_manager.hpp
            src/resource_core/resource_manager.cpp
            src/resource_core/file_handle.cpp
            src/resource_core/resource_error.cpp
            tests/tests.cpp
            tests/tests.cpp
            tests/tests.cpp
            tests/tests.cpp
            tests/tests.cpp
            tests/tests.cpp)
    target_compile_options(project_sanitizers INTERFACE $<$<CONFIG:Debug>:-fsanitize=address -fno-omit-frame-pointer>)
    target_link_options(project_sanitizers INTERFACE    $<$<CONFIG:Debug>:-fsanitize=address>)
endif ()

add_subdirectory(src/resource_core)
add_subdirectory(tests)
