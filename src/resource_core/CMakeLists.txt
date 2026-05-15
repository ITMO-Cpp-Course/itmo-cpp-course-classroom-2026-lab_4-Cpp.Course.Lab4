add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_library(resource_core STATIC 
    "src/ResourceManager.cpp"
    "src/FileHandle.cpp"
    "src/resource_err.cpp"
)

target_link_libraries(resource_core PRIVATE project_warnings)

if (TARGET project_sanitizers)
    target_link_libraries(resource_core PRIVATE project_sanitizers)
endif()

target_include_directories(resource_core
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)