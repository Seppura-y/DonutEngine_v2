cmake_minimum_required(VERSION 3.22)
project("SandboxScriptBuild" CSharp)

set(STARTUP_PROJ "SandboxScript" CACHE STRING "set the vs startup project")

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/SandboxScript)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/DonutScriptCore)

add_dependencies(SandboxScript DonutScriptCore)

if(STARTUP_PROJ)
    set_property(DIRECTORY ${CMAKE_CURRENT_LIST_DIR} PROPERTY VS_STARTUP_PROJECT ${STARTUP_PROJ})
endif()