add_executable(date_cli
        src/main.cpp
)

target_link_libraries(date_cli PRIVATE date_core project_warnings)
if (TARGET project_sanitizers)
    target_link_libraries(date_cli PRIVATE project_sanitizers)
endif ()
