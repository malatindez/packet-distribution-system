file(GLOB_RECURSE SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.*"
)
file(GLOB_RECURSE COMMON_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/../common/*.*"
)

update_sources_msvc(${SOURCES})
update_sources_msvc(${COMMON_SOURCES})
add_executable(central_server ${SOURCES} ${COMMON_SOURCES})

target_link_libraries(central_server PUBLIC mal-packet-weaver)

find_package(Boost REQUIRED COMPONENTS system thread filesystem program_options date_time serialization regex context coroutine HINTS "
  C:/" 
  "C:/Boost" 
  "${CMAKE_CURRENT_SOURCE_DIR}/third_party/boost")
  
target_include_directories(central_server PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(central_server PUBLIC ${Boost_LIBRARIES})


target_include_directories(central_server PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../common/")
target_set_output_directory(central_server)

if (MSVC)
  target_compile_options(central_server PRIVATE /bigobj)
else ()
  target_compile_options(central_server PRIVATE -Wa,-mbig-obj)
endif ()