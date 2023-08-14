file(GLOB_RECURSE SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.*"
)
update_sources_msvc(${SOURCES})

add_executable(keygen ${SOURCES})

set(CMAKE_CXX_FLAGS_DEBUG_INIT "${CMAKE_CXX_FLAGS_DEBUG_INIT}" "Zl")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /INCREMENTAL /EDITANDCONTINUE")

target_link_libraries(keygen PUBLIC mal-packet-weaver)

find_package(Boost REQUIRED COMPONENTS system thread filesystem program_options date_time serialization regex context coroutine HINTS "
  C:/" 
  "C:/Boost" 
  "${CMAKE_CURRENT_SOURCE_DIR}/third_party/boost")
  
target_include_directories(keygen PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(keygen PUBLIC ${Boost_LIBRARIES})

set_target_properties(keygen PROPERTIES LINKER_LANGUAGE CXX)

target_set_output_directory(keygen)