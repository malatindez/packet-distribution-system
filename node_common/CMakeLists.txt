set(NODE_COMMON_SOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/node_common")
file(GLOB_RECURSE CPP_SOURCES ${NODE_COMMON_SOURCES_DIR} *.*)
list(FILTER CPP_SOURCES INCLUDE REGEX ${NODE_COMMON_SOURCES_DIR}/*)
set(NODE_COMMON_SOURCES ${CPP_SOURCES})

foreach(_source IN ITEMS ${NODE_COMMON_SOURCES})
  if(IS_ABSOLUTE "${_source}")
    file(RELATIVE_PATH _source_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source}")
  else()
    set(_source_rel "${_source}")
  endif()

  get_filename_component(_source_path "${_source_rel}" PATH)
  string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
  source_group("${_source_path_msvc}" FILES "${_source}")
endforeach()

add_library(node_common STATIC ${NODE_COMMON_SOURCES})
target_add_libraries(node_common "openssl" "spdlog" "boost")
target_include_directories(node_common PUBLIC ${NODE_COMMON_SOURCES_DIR})

set(CMAKE_CXX_FLAGS_DEBUG_INIT "${CMAKE_CXX_FLAGS_DEBUG_INIT}" "Zl")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /INCREMENTAL /EDITANDCONTINUE")
set_target_properties(node_common PROPERTIES LINKER_LANGUAGE CXX)