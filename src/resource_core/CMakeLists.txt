add_library(resource_core
        src/FileHandle.cpp
        src/ResourceManager.cpp
)

target_include_directories(resource_core PUBLIC include)

target_link_libraries(resource_core PRIVATE project_warnings)

if (TARGET project_sanitizers)
    target_link_libraries(resource_core PRIVATE project_sanitizers)
endif ()