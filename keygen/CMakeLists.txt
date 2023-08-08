set(CPP_SOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/keygen")

set(CMAKE_CXX_STANDARD 20)
file(GLOB_RECURSE CPP_SOURCES ${CPP_SOURCES_DIR} *.*)
list(FILTER CPP_SOURCES INCLUDE REGEX ${CPP_SOURCES_DIR}/*)
set(KEYGEN_SOURCES ${CPP_SOURCES})
set(PROJECT_NAME Keygen)

IF (WIN32)
foreach(_source IN ITEMS ${KEYGEN_SOURCES})
  if(IS_ABSOLUTE "${_source}")
    file(RELATIVE_PATH _source_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source}")
  else()
    set(_source_rel "${_source}")
  endif()

  get_filename_component(_source_path "${_source_rel}" PATH)
  string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
  source_group("${_source_path_msvc}" FILES "${_source}")
endforeach()
ENDIF()

add_executable(${PROJECT_NAME} ${KEYGEN_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${KEYGEN_SOURCES_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${NODE_COMMON_SOURCES_DIR})
set(CMAKE_CXX_FLAGS_DEBUG_INIT "${CMAKE_CXX_FLAGS_DEBUG_INIT}" "Zl")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /INCREMENTAL /EDITANDCONTINUE")

target_link_libraries(${PROJECT_NAME} node_common)

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_set_output_directory(${PROJECT_NAME})