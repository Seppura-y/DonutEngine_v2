cmake_minimum_required(VERSION 3.22)

project(DonutEngine LANGUAGES CXX)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/dependencies/glad)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/dependencies/glfw)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/dependencies/imgui)
set_property(TARGET glad glfw imgui PROPERTY FOLDER "Dependencies")

set(DN_SHARED ON CACHE BOOL "Build shared library?")

set(GLFW_BUILD_DOCS OFF CACHE BOOL "GLFW lib only")
set(GLFW_INSTALL OFF CACHE BOOL "GLFW lib only")

set(RUNTIME_DIR ${CMAKE_CURRENT_LIST_DIR}/../../bin)
set(LIBRARY_DIR ${CMAKE_CURRENT_LIST_DIR}/../../lib)

set(USE_PCH true)

#===================================== CONFIGURATION TYPES OF THE PROJECT ==================================#
set(CONFIG_TYPE_DEBUG TRUE CACHE BOOL "Debug" )
set(CONFIG_TYPE_RELEASE FALSE CACHE BOOL "Release" )
set(CONFIG_TYPE_RELWITHDEBINFO FALSE CACHE BOOL "RelWithDebInfo" )
set(CONFIG_TYPE_MINSIZEREL FALSE CACHE BOOL "MinSizeRel")

if(${CONFIG_TYPE_DEBUG})
    list(APPEND CONFIG_TYPES "Debug;")
endif()

if(${CONFIG_TYPE_RELEASE})
    list(APPEND CONFIG_TYPES "Release;")
endif()

if(${CONFIG_TYPE_RELWITHDEBINFO})
    list(APPEND CONFIG_TYPES "RelWithDebInfo;")
endif()

if(${CONFIG_TYPE_MINSIZEREL})
    list(APPEND CONFIG_TYPES "MinSizeRel")
endif()

set(CMAKE_CONFIGURATION_TYPES ${CONFIG_TYPES})


#============================================== INCLUSION OF Qt ==============================================#
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_PREFIX_PATH $ENV{Qt515_64})
# set(AUTOGEN_BUILD_DIR ${CMAKE_CURRENT_LIST_DIR}/gens)

find_package(Qt5 COMPONENTS 
    Core
    Widgets 
    OpenGL
    OpenGLExtensions
    REQUIRED
)

#========================================= INCLUSION OF Project Files =========================================#
set(FORMS_DIR ${CMAKE_CURRENT_LIST_DIR}/forms)

LIST(APPEND SOURCES_DIR 
    ${CMAKE_CURRENT_LIST_DIR}/src
    ${CMAKE_CURRENT_LIST_DIR}/src/config
    ${CMAKE_CURRENT_LIST_DIR}/src/tabs
    ${CMAKE_CURRENT_LIST_DIR}/src/dialogs
    ${CMAKE_CURRENT_LIST_DIR}/src/control_bar
    ${CMAKE_CURRENT_LIST_DIR}/src/splitter_widget
    ${CMAKE_CURRENT_LIST_DIR}/src/video_renderer
    ${CMAKE_CURRENT_LIST_DIR}/src/video_player
)

include_directories(${FORMS_DIR})


FILE(GLOB PRECOMPILE_HEADER
    ${CMAKE_CURRENT_LIST_DIR}/src/pch.h
)

FILE(GLOB ${PROJECT_NAME}_INTERFACE_HEADER
    ${CMAKE_CURRENT_LIST_DIR}/src/donut.h
)

#####################################################################################################
# Global
FILE(GLOB ${PROJECT_NAME}_GLOBAL_HEADER
    ${CMAKE_CURRENT_LIST_DIR}/src/donut.h
    ${CMAKE_CURRENT_LIST_DIR}/src/pch.h
)

FILE(GLOB ${PROJECT_NAME}_GLOBAL_SRC
    ${CMAKE_CURRENT_LIST_DIR}/src/pch.cpp
)
#####################################################################################################
# Core
FILE(GLOB ${PROJECT_NAME}_CORE_HEADER
    ${CMAKE_CURRENT_LIST_DIR}/src/core/*.h
)

FILE(GLOB ${PROJECT_NAME}_CORE_SRC
    ${CMAKE_CURRENT_LIST_DIR}/src/core/*.cpp
)

#####################################################################################################
# Event
FILE(GLOB ${PROJECT_NAME}_EVENTS_HEADER
    ${CMAKE_CURRENT_LIST_DIR}/src/events/*.h
)

#####################################################################################################
# Renderer
FILE(GLOB ${PROJECT_NAME}_RENDERER_HEADER
    ${CMAKE_CURRENT_LIST_DIR}/src/renderer/*.h
)

FILE(GLOB ${PROJECT_NAME}_RENDERER_SRC
    ${CMAKE_CURRENT_LIST_DIR}/src/renderer/*.cpp
)
#####################################################################################################
# OpenGL
FILE(GLOB ${PROJECT_NAME}_OPENGL_HEADER
    ${CMAKE_CURRENT_LIST_DIR}/src/platform/opengl/*.h
)

FILE(GLOB ${PROJECT_NAME}_OPENGL_SRC
    ${CMAKE_CURRENT_LIST_DIR}/src/platform/opengl/*.cpp
)

#####################################################################################################
# Windows
FILE(GLOB ${PROJECT_NAME}_WINDOWS_HEADER
    ${CMAKE_CURRENT_LIST_DIR}/src/platform/windows/*.h
)

FILE(GLOB ${PROJECT_NAME}_WINDOWS_SRC
    ${CMAKE_CURRENT_LIST_DIR}/src/platform/windows/*.cpp
)

#####################################################################################################
# imgui
FILE(GLOB ${PROJECT_NAME}_IMGUI_HEADER
    ${CMAKE_CURRENT_LIST_DIR}/src/imgui/*.h
)

FILE(GLOB ${PROJECT_NAME}_IMGUI_SRC
    ${CMAKE_CURRENT_LIST_DIR}/src/imgui/*.cpp
)

#####################################################################################################
source_group("src\\DonutEngine\\Global" FILES ${${PROJECT_NAME}_GLOBAL_HEADER} ${${PROJECT_NAME}_GLOBAL_SRC})
source_group("src\\DonutEngine\\Core" FILES ${${PROJECT_NAME}_CORE_HEADER} ${${PROJECT_NAME}_CORE_SRC})
source_group("src\\DonutEngine\\Events" FILES ${${PROJECT_NAME}_EVENTS_HEADER} ${${PROJECT_NAME}_EVENTS_SRC})
source_group("src\\DonutEngine\\Renderer" FILES ${${PROJECT_NAME}_RENDERER_HEADER} ${${PROJECT_NAME}_RENDERER_SRC})
source_group("src\\DonutEngine\\ImGui" FILES ${${PROJECT_NAME}_IMGUI_HEADER} ${${PROJECT_NAME}_IMGUI_SRC})

source_group("src\\DonutEngine\\Platform\\OpenGL" FILES ${${PROJECT_NAME}_OPENGL_HEADER} ${${PROJECT_NAME}_OPENGL_SRC})
source_group("src\\DonutEngine\\Platform\\Windows" FILES ${${PROJECT_NAME}_WINDOWS_HEADER} ${${PROJECT_NAME}_WINDOWS_SRC})

#========================================= SETUP LIBRARY =========================================#
# Enable debug logging on RELWITHDEBINFO configuration
set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS
    $<$<CONFIG:RELWITHDEBINFO>:QT_MESSAGELOGCONTEXT>
)

# Add the forms directory to the AUTOUIC search paths
# set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_AUTOUIC_SEARCH_PATHS} ${FORMS_DIR})

# Set the startup project for visual studio
set_property(DIRECTORY ${CMAKE_CURRENT_LIST_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

if(DN_SHARED)
    set(CONFIG_TYPE SHARED)
else()
    set(CONFIG_TYPE STATIC)
endif()


# Add the library
if (WIN32) 
    add_library(${PROJECT_NAME} ${CONFIG_TYPE} 
        ${GLOBAL_HEADER}
        ${GLOBAL_SRC}

        ${${PROJECT_NAME}_GLOBAL_HEADER} ${${PROJECT_NAME}_GLOBAL_SRC}
        ${${PROJECT_NAME}_CORE_HEADER} ${${PROJECT_NAME}_CORE_SRC}
        ${${PROJECT_NAME}_EVENTS_HEADER} ${${PROJECT_NAME}_EVENTS_SRC}
        ${${PROJECT_NAME}_RENDERER_HEADER} ${${PROJECT_NAME}_RENDERER_SRC}
        ${${PROJECT_NAME}_OPENGL_HEADER} ${${PROJECT_NAME}_OPENGL_SRC}
        ${${PROJECT_NAME}_WINDOWS_HEADER} ${${PROJECT_NAME}_WINDOWS_SRC}
        ${${PROJECT_NAME}_IMGUI_HEADER} ${${PROJECT_NAME}_IMGUI_SRC}
    )
elseif(UNIX)
    add_library(${PROJECT_NAME} 
        ${GLOBAL_HEADER}
        ${GLOBAL_SRC}

        ${${PROJECT_NAME}_GLOBAL_HEADER} ${${PROJECT_NAME}_GLOBAL_SRC}
        ${${PROJECT_NAME}_CORE_HEADER} ${${PROJECT_NAME}_CORE_SRC}
        ${${PROJECT_NAME}_EVENTS_HEADER} ${${PROJECT_NAME}_EVENTS_SRC}
        ${${PROJECT_NAME}_RENDERER_HEADER} ${${PROJECT_NAME}_RENDERER_SRC}
        ${${PROJECT_NAME}_OPENGL_HEADER} ${${PROJECT_NAME}_OPENGL_SRC}
        ${${PROJECT_NAME}_WINDOWS_HEADER} ${${PROJECT_NAME}_WINDOWS_SRC}
    )
endif()

if(USE_PCH)
    target_precompile_headers(${PROJECT_NAME}
        PRIVATE
        ${PRECOMPILE_HEADER}
    )
endif()

set(PUB_HEADERS         
    ${CMAKE_CURRENT_LIST_DIR}/src/donut.h
    ${CMAKE_CURRENT_LIST_DIR}/src/core/core.h
    ${CMAKE_CURRENT_LIST_DIR}/src/core/application.h
    ${CMAKE_CURRENT_LIST_DIR}/src/core/engine_log.h
    ${CMAKE_CURRENT_LIST_DIR}/src/core/window.h
)
set_target_properties(${PROJECT_NAME} PROPERTIES 
    PUBLIC_HEADER "${PUB_HEADERS}"
    # VERSION ${PROJECT_VERSION}
)

# set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "frontend")

if( MSVC )

    if(DN_SHARED)
        target_compile_definitions(${PROJECT_NAME} PRIVATE
            DN_BUILD_SHARED
        )
    else()
        target_compile_definitions(${PROJECT_NAME} PRIVATE
            DN_BUILD_STATIC
        )
    endif()

    set_target_properties(${PROJECT_NAME} PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/../../bin
    )
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE
    __STDC_CONSTANT_MACROS
    DN_PLATFORM_WINDOWS
    DN_BUILD_SHARED
    DN_USE_GLFW
    GLFW_INCLUDE_NONE
    # if(DN_SHARED)
    #     DN_BUILD_SHARED
    # endif()
)

target_compile_features(${PROJECT_NAME} PRIVATE
    cxx_std_17
)

# Add the target includes for the project 
target_include_directories(${PROJECT_NAME} PRIVATE ${FORMS_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${SOURCES_DIR})

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_LIST_DIR}/src
    ${CMAKE_CURRENT_LIST_DIR}/src/core
    ${CMAKE_CURRENT_LIST_DIR}/src/renderer

    ${CMAKE_CURRENT_LIST_DIR}/../../dependencies
    ${CMAKE_CURRENT_LIST_DIR}/../../dependencies/spdlog/include
    ${CMAKE_CURRENT_LIST_DIR}/../../dependencies/glfw/include
    ${CMAKE_CURRENT_LIST_DIR}/../../dependencies/glad/include
    ${CMAKE_CURRENT_LIST_DIR}/../../dependencies/glm
    ${CMAKE_CURRENT_LIST_DIR}/../../dependencies/imgui
)

#========================================= LINKING LIBRARIES =========================================#
target_link_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../../lib
)

target_link_libraries(${PROJECT_NAME} 
    Qt5::Core
    Qt5::Widgets
    Qt5::OpenGL
    Qt5::OpenGLExtensions
    user32.lib
    opengl32.lib
    dwmapi.lib
    libvlc.lib
    libvlccore.lib
    DonutAVLibrary
    SDL2
    SDL2main
    avcodec
    glfw
    glad
    imgui
)

foreach(type IN LISTS CMAKE_CONFIGURATION_TYPES)
    if(type)
        string(TOUPPER ${type} config)
        message("${PROJECT_NAME} config = ${config}")
        set(CMAKE_CXX_FLAGS_${config} "${CMAKE_CXX_FLAGS_${config}} -D${config}")
    endif()

    set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_${config} ${RUNTIME_DIR}   #dll exe 
        LIBRARY_OUTPUT_DIRECTORY_${config} ${LIBRARY_DIR}   #so dylib
        ARCHIVE_OUTPUT_DIRECTORY_${config} ${LIBRARY_DIR}   #lib a
        PDB_OUTPUT_DIRECTORY_${config}     ${RUNTIME_DIR}   #pdb
    )
endforeach()

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_DEBUG_WORKING_DIRECTORY $<IF:$<CONFIG:Debug>,Debug,Release>
    DEBUG_POSTFIX "d"
)

set(INSTALL_PATH 
${CMAKE_INSTALL_PREFIX}/${CMAKE_VS_PLATFORM_NAME})


set(windeployqt "$ENV{Qt515_64}/bin/windeployqt")

install( CODE 
        "execute_process(COMMAND 
        ${windeployqt} $<TARGET_FILE:${PROJECT_NAME}>
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        )"
)

install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    PUBLIC_HEADER DESTINATION include/DonutEngine
    PRIVATE_HEADER DESTINATION inc
)

# install(EXPORT ${PROJECT_NAME} FILE "${PROJECT_NAME}Config.cmake" DESTINATION config)

install(DIRECTORY 
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
    DESTINATION
    ${INSTALL_PATH}
)