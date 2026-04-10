add_library(resource_core STATIC
        file_handle.cpp
        resource_manager.cpp
        include/resource_core/resource_error.hpp
        include/resource_core/file_handle.hpp
        include/resource_core/resource_manager.hpp
)

target_link_libraries(resource_core PRIVATE project_warnings)
if (TARGET project_sanitizers)
        target_link_libraries(resource_core PRIVATE project_sanitizers)
endif ()

target_include_directories(resource_core
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
