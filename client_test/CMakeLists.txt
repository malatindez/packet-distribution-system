set(CPP_SOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/client_test")
file(GLOB_RECURSE CPP_SOURCES ${CPP_SOURCES_DIR} *.*)
list(FILTER CPP_SOURCES INCLUDE REGEX ${CPP_SOURCES_DIR}/*)
set(CLIENT_TEST_SOURCES ${CPP_SOURCES})

foreach(_source IN ITEMS ${CLIENT_TEST_SOURCES})
  if(IS_ABSOLUTE "${_source}")
    file(RELATIVE_PATH _source_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source}")
  else()
    set(_source_rel "${_source}")
  endif()

  get_filename_component(_source_path "${_source_rel}" PATH)
  string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
  source_group("${_source_path_msvc}" FILES "${_source}")
endforeach()

add_executable(client_test ${CLIENT_TEST_SOURCES})


target_include_directories(client_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/client_test)
target_link_libraries(client_test PRIVATE client_dll)

add_custom_command(TARGET client_test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "$<TARGET_FILE:client_dll>"
    "$<TARGET_FILE_DIR:client_test>"
)

