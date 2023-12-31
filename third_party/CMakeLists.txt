set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party")

set(GTEST_DIR "${LIB_DIR}/google/google_test")
set(MAL_PACKET_WEAVER_DIR "${LIB_DIR}/mal-packet-weaver")

set(OPENSSL_USE_STATIC_LIBS TRUE)
find_package(OpenSSL REQUIRED)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
add_subdirectory("${GTEST_DIR}" EXCLUDE_FROM_ALL)
add_subdirectory("${MAL_PACKET_WEAVER_DIR}" EXCLUDE_FROM_ALL)

set_target_properties(gtest PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${BINARY_OUTPUT_DIR})
set_target_properties(gtest PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BINARY_OUTPUT_DIR})

set_target_properties(gtest_main PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${BINARY_OUTPUT_DIR})
set_target_properties(gtest_main PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BINARY_OUTPUT_DIR})

function(target_add_gtest TARGET)
  enable_testing()
  include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
  include_directories(${INCLUDES})
  target_link_libraries("${TARGET}" PUBLIC gtest gtest_main)
endfunction(target_add_gtest)

function(target_add_libraries TARGET)
  if("${ARGN}" MATCHES "[gG][tT][eE][sS][tT]")
    target_add_gtest(${TARGET})
  endif()
endfunction(target_add_libraries)
