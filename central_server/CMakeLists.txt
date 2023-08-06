set(CPP_SOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/central_server")
file(GLOB_RECURSE CPP_SOURCES ${CPP_SOURCES_DIR} *.*)
list(FILTER CPP_SOURCES INCLUDE REGEX ${CPP_SOURCES_DIR}/*)
set(CENTRAL_SERVER_SOURCES ${CPP_SOURCES})

foreach(_source IN ITEMS ${CENTRAL_SERVER_SOURCES})
  if(IS_ABSOLUTE "${_source}")
    file(RELATIVE_PATH _source_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source}")
  else()
    set(_source_rel "${_source}")
  endif()

  get_filename_component(_source_path "${_source_rel}" PATH)
  string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
  source_group("${_source_path_msvc}" FILES "${_source}")
endforeach()

add_executable(central_server ${CENTRAL_SERVER_SOURCES})

target_include_directories(central_server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/third_party/boost)

target_include_directories(central_server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/central_server)

