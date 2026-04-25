add_library(resource_core STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/resource_error.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/file_handle.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/file_io.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/resource_manager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/resource_core/resource_error.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/resource_core/file_handle.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/resource_core/file_io.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/resource_core/resource_manager.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/resource_core/resource_core.hpp
)