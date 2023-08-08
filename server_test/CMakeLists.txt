set(CPP_SOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/server_test")
file(GLOB_RECURSE CPP_SOURCES ${CPP_SOURCES_DIR} *.*)
list(FILTER CPP_SOURCES INCLUDE REGEX ${CPP_SOURCES_DIR}/*)
set(SERVER_TEST_SOURCES ${CPP_SOURCES})
set(PROJECT_NAME server_test)

foreach(_source IN ITEMS ${SERVER_TEST_SOURCES})
  if(IS_ABSOLUTE "${_source}")
    file(RELATIVE_PATH _source_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source}")
  else()
    set(_source_rel "${_source}")
  endif()

  get_filename_component(_source_path "${_source_rel}" PATH)
  string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
  source_group("${_source_path_msvc}" FILES "${_source}")
endforeach()

add_executable(${PROJECT_NAME} ${SERVER_TEST_SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE server_dll)


target_set_output_directory(${PROJECT_NAME})