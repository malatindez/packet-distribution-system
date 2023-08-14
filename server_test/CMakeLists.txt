file(GLOB_RECURSE SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.*"
)

update_sources_msvc(${SOURCES})

add_executable(server_test ${SOURCES})

target_link_libraries(server_test PRIVATE server_dll)

target_set_output_directory(server_test)