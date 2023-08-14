file(GLOB_RECURSE SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.*"
)

update_sources_msvc(${SOURCES})

add_library(server_dll SHARED ${SOURCES})

target_link_libraries(server_dll PUBLIC mal-packet-weaver)
target_include_directories(server_dll PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../common/")

set(CMAKE_CXX_FLAGS_DEBUG_INIT "${CMAKE_CXX_FLAGS_DEBUG_INIT}" "Zl")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /INCREMENTAL /EDITANDCONTINUE")

target_compile_definitions(server_dll PUBLIC SERVER_DLL_EXPORTS)
 
set_target_properties(server_dll PROPERTIES LINKER_LANGUAGE CXX)

target_set_output_directory(server_dll)