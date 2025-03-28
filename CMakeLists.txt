#cmake -S . -B build -A x64 -D CMAKE_INSTALL_PREFIX=./ -D QT5=G:/application/Qt/Qt5.15/5.15.2/msvc2019_64
#cmake --build build --config Release
#cmake --install build --config Release
cmake_minimum_required(VERSION 3.22)
project(DonutEngine_v2_Build)


set(STARTUP_PROJ "DonutEditor" CACHE STRING "set the vs startup project")

set(PROJECT_DONUT_ENGINE            TRUE CACHE BOOL "add project DonutEngine ?")
set(PROJECT_DONUT_EDITOR            TRUE CACHE BOOL "add project DonutEditor ?")

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/src/DonutEngine)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/src/DonutEditor)
# add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/src/DonutScripts/DonutScriptCore)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/src/DonutScripts)

add_dependencies(DonutEditor DonutEngine)
add_dependencies(DonutEngine glad glfw imgui yaml-cpp Box2D msdf-atlas-gen msdfgen freetype DonutScriptCore SandboxScript)

if(STARTUP_PROJ)
    set_property(DIRECTORY ${CMAKE_CURRENT_LIST_DIR} PROPERTY VS_STARTUP_PROJECT ${STARTUP_PROJ})
endif()
