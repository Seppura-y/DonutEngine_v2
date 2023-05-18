#cmake -S . -B build -A x64 -D CMAKE_INSTALL_PREFIX=./ -D QT5=G:/application/Qt/Qt5.15/5.15.2/msvc2019_64
#cmake --build build --config Release
#cmake --install build --config Release
cmake_minimum_required(VERSION 3.22)
project(DonutEngine_v2_Build)


set(STARTUP_PROJ "DonuEditor" CACHE STRING "set the vs startup project")

set(PROJECT_GLAD              TRUE CACHE BOOL "add project GLAD ?")
set(PROJECT_GLFW            TRUE CACHE BOOL "add project GLFW ?")
set(PROJECT_DONUT_ENGINE            TRUE CACHE BOOL "add project DonutEngine ?")
set(PROJECT_DONUT_AV_LIBRARY            TRUE CACHE BOOL "add project DonutAVLibrary ?")
set(PROJECT_DONUT_MAIN_WIDGET            TRUE CACHE BOOL "add project DonutMainWidget ?")


# if(PROJECT_DONUT_AV_LIBRARY)
#     add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/src/DonutAVLibrary)
# endif()

# if(PROJECT_DONUT_CMR_MANAGER)
#     add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/src/DonutCmrManager)
# endif()

# add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/dependencies/glfw)
# add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/dependencies/glad)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/src/DonutEngine)
# add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/src/DonutAVLibrary)
# add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/src/DonutMainWidget)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/src/DonutEditor)

# add_dependencies(DonutMainWidget DonutEngine DonutAVLibrary)
add_dependencies(DonutEditor DonutEngine )

if(STARTUP_PROJ)
    set_property(DIRECTORY ${CMAKE_CURRENT_LIST_DIR} PROPERTY VS_STARTUP_PROJECT ${STARTUP_PROJ})
endif()


# add_dependencies(DonutEditorFramework)
