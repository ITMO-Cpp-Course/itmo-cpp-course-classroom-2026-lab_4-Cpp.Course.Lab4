add_library(resource_core STATIC
    resource_error.cpp
    file_handle.cpp
    resource_manager.cpp)
target_link_libraries(resource_core PRIVATE project_warnings)
if (TARGET project_sanitizers)
        target_link_libraries(resource_core PRIVATE project_sanitizers)
endif ()

target_include_directories(resource_core
        PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)
