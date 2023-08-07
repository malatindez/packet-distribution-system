set(CPP_SOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/server_dll")
file(GLOB_RECURSE CPP_SOURCES ${CPP_SOURCES_DIR} *.*)
list(FILTER CPP_SOURCES INCLUDE REGEX ${CPP_SOURCES_DIR}/*)
set(SERVER_DLL_SOURCES ${CPP_SOURCES})
set(PROJECT_NAME server_dll)

foreach(_source IN ITEMS ${SERVER_DLL_SOURCES})
  if(IS_ABSOLUTE "${_source}")
    file(RELATIVE_PATH _source_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source}")
  else()
    set(_source_rel "${_source}")
  endif()

  get_filename_component(_source_path "${_source_rel}" PATH)
  string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
  source_group("${_source_path_msvc}" FILES "${_source}")
endforeach()

add_library(${PROJECT_NAME} SHARED ${SERVER_DLL_SOURCES})
target_link_libraries(${PROJECT_NAME} PUBLIC node_common)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/node_common)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/third_party/boost)

set(CMAKE_CXX_FLAGS_DEBUG_INIT "${CMAKE_CXX_FLAGS_DEBUG_INIT}" "Zl")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /INCREMENTAL /EDITANDCONTINUE")

target_compile_definitions(${PROJECT_NAME} PUBLIC SERVER_DLL_EXPORTS)

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)