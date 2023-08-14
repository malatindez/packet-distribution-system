file(GLOB_RECURSE SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.*"
)
update_sources_msvc(${SOURCES})
add_executable(central_server ${SOURCES})

target_link_libraries(central_server PUBLIC mal-packet-weaver)

find_package(Boost REQUIRED COMPONENTS system thread filesystem program_options date_time serialization regex context coroutine HINTS "
  C:/" 
  "C:/Boost" 
  "${CMAKE_CURRENT_SOURCE_DIR}/third_party/boost")
  
target_include_directories(central_server PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(central_server PUBLIC ${Boost_LIBRARIES})


target_include_directories(central_server PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../common/")
target_set_output_directory(central_server)