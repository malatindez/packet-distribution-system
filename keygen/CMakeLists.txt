file(GLOB_RECURSE SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.*"
)
update_sources_msvc(${SOURCES})

add_executable(keygen ${SOURCES})

set(CMAKE_CXX_FLAGS_DEBUG_INIT "${CMAKE_CXX_FLAGS_DEBUG_INIT}" "Zl")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /INCREMENTAL /EDITANDCONTINUE")

target_link_libraries(client_dll PUBLIC mal-packet-weaver)

set_target_properties(keygen PROPERTIES LINKER_LANGUAGE CXX)

target_set_output_directory(keygen)