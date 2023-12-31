cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(packet-distribution-system LANGUAGES CXX)

add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Rpass=inline -Winline -W4")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Winline /W4")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /W4")
endif()



if(MSVC)
  add_compile_options("$<$<CONFIG:RELEASE>:/MP>")
  add_compile_options("$<$<CONFIG:RELEASE>:/Oi>")
  add_compile_options("$<$<CONFIG:RELEASE>:/Ot>")
  add_compile_options("$<$<CONFIG:RELEASE>:/GL>")
  add_compile_options("$<$<CONFIG:RELEASE>:/sdl->")
  add_compile_options("$<$<CONFIG:RELEASE>:/arch:AVX2>") 
  add_compile_options("$<$<CONFIG:RELEASE>:/GS->")  
  add_compile_options("$<$<CONFIG:RELEASE>:/fp:precise>") 
  add_compile_options("$<$<CONFIG:RELEASE>:/Oy>")
  add_compile_options("$<$<CONFIG:RELEASE>:/GT>")
  
  add_compile_options("$<$<CONFIG:RelWithDebInfo>:/MP>")
  add_compile_options("$<$<CONFIG:RelWithDebInfo>:/Oi>")
  add_compile_options("$<$<CONFIG:RelWithDebInfo>:/Ot>")
  add_compile_options("$<$<CONFIG:RelWithDebInfo>:/GL>")
  add_compile_options("$<$<CONFIG:RelWithDebInfo>:/sdl->")
  add_compile_options("$<$<CONFIG:RelWithDebInfo>:/arch:AVX2>") 
  add_compile_options("$<$<CONFIG:RelWithDebInfo>:/GS->")  
  add_compile_options("$<$<CONFIG:RelWithDebInfo>:/fp:precise>") 
  add_compile_options("$<$<CONFIG:RelWithDebInfo>:/Oy>")
  add_compile_options("$<$<CONFIG:RelWithDebInfo>:/GT>")
  
  add_compile_options("$<$<CONFIG:MinSizeRel>:/MP>")
  add_compile_options("$<$<CONFIG:MinSizeRel>:/Oi>")
  add_compile_options("$<$<CONFIG:MinSizeRel>:/Ot>")
  add_compile_options("$<$<CONFIG:MinSizeRel>:/GL>")
  add_compile_options("$<$<CONFIG:MinSizeRel>:/sdl->")
  add_compile_options("$<$<CONFIG:MinSizeRel>:/arch:AVX2>") 
  add_compile_options("$<$<CONFIG:MinSizeRel>:/GS->")  
  add_compile_options("$<$<CONFIG:MinSizeRel>:/fp:precise>") 
  add_compile_options("$<$<CONFIG:MinSizeRel>:/Oy>")
  add_compile_options("$<$<CONFIG:MinSizeRel>:/GT>")
  
  add_compile_options("$<$<CONFIG:Debug>:/fp:precise>") 

  add_compile_options(/W4 /WX)

endif()
if (WIN32)
  add_compile_definitions(_WIN32_WINNT=0x0A00)
endif()


set (MAIN_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

make_directory(${CMAKE_BINARY_DIR}/binary)
set(BINARY_OUTPUT_DIR ${CMAKE_BINARY_DIR}/binary)




function(target_set_output_directory TARGET)
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}.vcxproj.user" "\
<?xml version=\"1.0\" encoding=\"utf-8\"?> \ 
<Project ToolsVersion=\"Current\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\"> \ 
  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Debug|x64'\"> \ 
    <LocalDebuggerWorkingDirectory>${MAIN_SRC_DIR}\\runtime_directory\\</LocalDebuggerWorkingDirectory> \ 
    <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor> \ 
  </PropertyGroup> \ 
  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='MinSizeRel|x64'\"> \ 
    <LocalDebuggerWorkingDirectory>${MAIN_SRC_DIR}\\runtime_directory\\</LocalDebuggerWorkingDirectory> \ 
    <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor> \ 
  </PropertyGroup> \ 
  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Release|x64'\"> \ 
    <LocalDebuggerWorkingDirectory>${MAIN_SRC_DIR}\\runtime_directory\\</LocalDebuggerWorkingDirectory> \ 
    <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor> \ 
  </PropertyGroup> \ 
  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='RelWithDebInfo|x64'\"> \ 
    <LocalDebuggerWorkingDirectory>${MAIN_SRC_DIR}\\runtime_directory\\</LocalDebuggerWorkingDirectory> \ 
    <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor> \ 
  </PropertyGroup> \ 
</Project> \ 
")

set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BINARY_OUTPUT_DIR})


# copy the executable to the runtime directory
add_custom_command(TARGET ${TARGET} POST_BUILD 
  COMMAND "${CMAKE_COMMAND}" -E copy_if_different
     "$<TARGET_FILE:${TARGET}>"
     "${MAIN_SRC_DIR}/runtime_directory/$<TARGET_FILE_NAME:${TARGET}>" 
  COMMENT "Copying the executable to output directory"
)

endfunction()

include("${CMAKE_CURRENT_SOURCE_DIR}/third_party/CMakeLists.txt")
function(update_sources_msvc SOURCES)
if(MSVC) # update filters in MSVC
  foreach(_source IN ITEMS ${SOURCES})
    if(IS_ABSOLUTE "${_source}")
      file(RELATIVE_PATH _source_rel "${MAIN_SRC_DIR}" "${_source}")
    else()
      set(_source_rel "${_source}")
    endif()

    get_filename_component(_source_path "${_source_rel}" PATH)
    string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
    source_group("${_source_path_msvc}" FILES "${_source}")
  endforeach()
endif()
endfunction(update_sources_msvc)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/central_server")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/server_dll")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/client_dll")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/server_test")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/client_test")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/keygen")