file(GLOB_RECURSE SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.*"
)

update_sources_msvc(${SOURCES})

add_executable(client_test ${SOURCES})

target_link_libraries(client_test PRIVATE client_dll)

target_set_output_directory(client_test)