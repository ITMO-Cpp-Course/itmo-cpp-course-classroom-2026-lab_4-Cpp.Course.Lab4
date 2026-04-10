add_library(resource_core STATIC
        ResourceError.cpp
        FileHandle.cpp
        ResourceManager.cpp
)

target_include_directories(resource_core
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

if (WIN32)
    target_compile_definitions(resource_core PRIVATE _CRT_SECURE_NO_WARNINGS)
endif ()
