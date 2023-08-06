set(CPP_SOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/server_test")
file(GLOB_RECURSE CPP_SOURCES ${CPP_SOURCES_DIR} *.*)
list(FILTER CPP_SOURCES INCLUDE REGEX ${CPP_SOURCES_DIR}/*)
set(SERVER_TEST_SOURCES ${CPP_SOURCES})

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

add_executable(server_test ${SERVER_TEST_SOURCES})

target_include_directories(server_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/server_test)
target_link_libraries(server_test PRIVATE server_dll)

add_custom_command(TARGET server_test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "$<TARGET_FILE:server_dll>"
    "$<TARGET_FILE_DIR:server_test>"
)
