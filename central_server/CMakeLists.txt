file(GLOB_RECURSE SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.*"
)
update_sources_msvc(${SOURCES})
add_executable(central_server ${SOURCES})

target_link_libraries(central_server PUBLIC mal-packet-weaver)
target_include_directories(central_server PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../common/")
target_set_output_directory(central_server)