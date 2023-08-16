file(GLOB_RECURSE SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.*"
)
file(GLOB_RECURSE COMMON_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/../common/*.*"
)

update_sources_msvc(${SOURCES})
update_sources_msvc(${COMMON_SOURCES})

add_library(client_dll SHARED ${SOURCES} ${COMMON_SOURCES})

target_link_libraries(client_dll PUBLIC mal-packet-weaver)

find_package(Boost REQUIRED COMPONENTS system thread filesystem program_options date_time serialization regex context coroutine HINTS "
  C:/" 
  "C:/Boost" 
  "${CMAKE_CURRENT_SOURCE_DIR}/third_party/boost")
  
target_include_directories(client_dll PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(client_dll PUBLIC ${Boost_LIBRARIES})

target_include_directories(client_dll PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../common/")

set(CMAKE_CXX_FLAGS_DEBUG_INIT "${CMAKE_CXX_FLAGS_DEBUG_INIT}" "Zl")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /INCREMENTAL /EDITANDCONTINUE")

target_compile_definitions(client_dll PUBLIC CLIENT_DLL_EXPORTS)

 
set_target_properties(client_dll PROPERTIES LINKER_LANGUAGE CXX)

target_set_output_directory(client_dll)

if (MSVC)
  target_compile_options(client_dll PRIVATE /bigobj)
else ()
  target_compile_options(client_dll PRIVATE -Wa,-mbig-obj)
endif ()