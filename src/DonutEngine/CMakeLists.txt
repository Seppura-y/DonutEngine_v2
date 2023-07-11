cmake_minimum_required(VERSION 3.22)

project(DonutEngine LANGUAGES CXX)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/dependencies/glad)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/dependencies/glfw)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/dependencies/imgui)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/dependencies/yaml-cpp)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/dependencies/Box2d)
# add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/dependencies/spdlog)
# add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/dependencies/glm)
set_property(TARGET glad glfw imgui yaml-cpp Box2D PROPERTY FOLDER "Dependencies")

set(DN_SHARED OFF CACHE BOOL "Build shared library?")

set(GLFW_BUILD_DOCS OFF CACHE BOOL "GLFW lib only")
set(GLFW_INSTALL OFF CACHE BOOL "GLFW lib only")

set(RUNTIME_DIR ${CMAKE_CURRENT_LIST_DIR}/../../bin)
set(LIBRARY_DIR ${CMAKE_CURRENT_LIST_DIR}/../../lib)

set(USE_PCH true)

#===================================== CONFIGURATION TYPES OF THE PROJECT ==================================#

list(APPEND CONFIG_TYPES "Debug;")
list(APPEND CONFIG_TYPES "Release;")
list(APPEND CONFIG_TYPES "RelWithDebInfo;")
list(APPEND CONFIG_TYPES "MinSizeRel")

set(CMAKE_CONFIGURATION_TYPES ${CONFIG_TYPES})



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
# Project
FILE(GLOB ${PROJECT_NAME}_PROJECT_HEADER
    ${CMAKE_CURRENT_LIST_DIR}/src/project/*.h
)

FILE(GLOB ${PROJECT_NAME}_PROJECT_SRC
    ${CMAKE_CURRENT_LIST_DIR}/src/project/*.cpp
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
# Platform Utils
FILE(GLOB ${PROJECT_NAME}_UTILS_HEADER
    ${CMAKE_CURRENT_LIST_DIR}/src/utils/*.h
)

FILE(GLOB ${PROJECT_NAME}_UTILS_SRC
    ${CMAKE_CURRENT_LIST_DIR}/src/utils/*.cpp
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
# glm
FILE(GLOB ${PROJECT_NAME}_GLM_HEADER
    ${CMAKE_CURRENT_LIST_DIR}/dependencies/glm/glm/*.hpp
    ${CMAKE_CURRENT_LIST_DIR}/dependencies/glm/glm/*.inl
)

#####################################################################################################
# stb_image
FILE(GLOB ${PROJECT_NAME}_STB_IMAGE_HEADER
    ${CMAKE_CURRENT_LIST_DIR}/dependencies/stb_image/stb_image.h
)

FILE(GLOB ${PROJECT_NAME}_STB_IMAGE_SRC
    ${CMAKE_CURRENT_LIST_DIR}/dependencies/stb_image/stb_image.cpp
)

#####################################################################################################
# imguizmo
FILE(GLOB ${PROJECT_NAME}_IMGUIZMO_HEADER
    ${CMAKE_CURRENT_LIST_DIR}/dependencies/ImGuizmo/ImGuizmo.h
)

FILE(GLOB ${PROJECT_NAME}_IMGUIZMO_SRC
    ${CMAKE_CURRENT_LIST_DIR}/dependencies/ImGuizmo/ImGuizmo.cpp
)

#####################################################################################################
# math
FILE(GLOB ${PROJECT_NAME}_MATH_HEADER
    ${CMAKE_CURRENT_LIST_DIR}/src/math/*.h
)

FILE(GLOB ${PROJECT_NAME}_MATH_SRC
    ${CMAKE_CURRENT_LIST_DIR}/src/math/*.cpp
)


#####################################################################################################
# assets
FILE(GLOB ${PROJECT_NAME}_TEXTURES
    ${CMAKE_CURRENT_LIST_DIR}/../../bin/assets/textures/*.*
)

FILE(GLOB ${PROJECT_NAME}_SHADERS
    ${CMAKE_CURRENT_LIST_DIR}/../../bin/assets/shaders/*.*
)

#####################################################################################################
# debug
FILE(GLOB ${PROJECT_NAME}_DEBUG_HEADER
    ${CMAKE_CURRENT_LIST_DIR}/src/debug/*.h
)

FILE(GLOB ${PROJECT_NAME}_DEBUG_SRC
    ${CMAKE_CURRENT_LIST_DIR}/src/debug/*.cpp
)

#####################################################################################################
# scene
FILE(GLOB ${PROJECT_NAME}_SCENE_HEADER
    ${CMAKE_CURRENT_LIST_DIR}/src/scene/*.h
)

FILE(GLOB ${PROJECT_NAME}_SCENE_SRC
    ${CMAKE_CURRENT_LIST_DIR}/src/scene/*.cpp
)

#####################################################################################################
# Scripting
FILE(GLOB ${PROJECT_NAME}_SCRIPT_ENGINE_HEADER
    ${CMAKE_CURRENT_LIST_DIR}/src/scripting/*.h
)

FILE(GLOB ${PROJECT_NAME}_SCRIPT_ENGINE_SRC
    ${CMAKE_CURRENT_LIST_DIR}/src/scripting/*.cpp
)

#####################################################################################################
# ui
FILE(GLOB ${PROJECT_NAME}_UI_HEADER
    ${CMAKE_CURRENT_LIST_DIR}/src/ui/*.h
)

#####################################################################################################
# Physics
FILE(GLOB ${PROJECT_NAME}_PHYSICS_HEADER
    ${CMAKE_CURRENT_LIST_DIR}/src/physics/*.h
)


#####################################################################################################
source_group("src\\DonutEngine\\Global" FILES ${${PROJECT_NAME}_GLOBAL_HEADER} ${${PROJECT_NAME}_GLOBAL_SRC})
source_group("src\\DonutEngine\\Core" FILES ${${PROJECT_NAME}_CORE_HEADER} ${${PROJECT_NAME}_CORE_SRC})
source_group("src\\DonutEngine\\Events" FILES ${${PROJECT_NAME}_EVENTS_HEADER} ${${PROJECT_NAME}_EVENTS_SRC})
source_group("src\\DonutEngine\\Project" FILES ${${PROJECT_NAME}_PROJECT_HEADER} ${${PROJECT_NAME}_PROJECT_SRC})
source_group("src\\DonutEngine\\Renderer" FILES ${${PROJECT_NAME}_RENDERER_HEADER} ${${PROJECT_NAME}_RENDERER_SRC})
source_group("src\\DonutEngine\\ImGui" FILES ${${PROJECT_NAME}_IMGUI_HEADER} ${${PROJECT_NAME}_IMGUI_SRC})
source_group("src\\DonutEngine\\Scene" FILES ${${PROJECT_NAME}_SCENE_HEADER} ${${PROJECT_NAME}_SCENE_SRC})
source_group("src\\DonutEngine\\Utils" FILES ${${PROJECT_NAME}_UTILS_HEADER} ${${PROJECT_NAME}_UTILS_SRC})
source_group("src\\DonutEngine\\Math" FILES ${${PROJECT_NAME}_MATH_HEADER} ${${PROJECT_NAME}_MATH_SRC})
source_group("src\\DonutEngine\\Scripting" FILES ${${PROJECT_NAME}_SCRIPT_ENGINE_HEADER} ${${PROJECT_NAME}_SCRIPT_ENGINE_SRC})
source_group("src\\DonutEngine\\UI" FILES ${${PROJECT_NAME}_UI_HEADER})
source_group("src\\DonutEngine\\Physics" FILES ${${PROJECT_NAME}_PHYSICS_HEADER})

source_group("src\\DonutEngine\\Platform\\OpenGL" FILES ${${PROJECT_NAME}_OPENGL_HEADER} ${${PROJECT_NAME}_OPENGL_SRC})
source_group("src\\DonutEngine\\Platform\\Windows" FILES ${${PROJECT_NAME}_WINDOWS_HEADER} ${${PROJECT_NAME}_WINDOWS_SRC})

source_group("src\\Debug" FILES ${${PROJECT_NAME}_DEBUG_HEADER} ${${PROJECT_NAME}_DEBUG_SRC})

source_group("src\\glm" FILES ${${PROJECT_NAME}_GLM_HEADER})

source_group("src\\stb_image" FILES ${${PROJECT_NAME}_STB_IMAGE_HEADER} ${${PROJECT_NAME}_STB_IMAGE_SRC})

source_group("src\\imguizmo" FILES ${${PROJECT_NAME}_IMGUIZMO_HEADER} ${${PROJECT_NAME}_IMGUIZMO_SRC})

source_group("assets\\shaders"  FILES ${${PROJECT_NAME}_SHADERS} )
source_group("assets\\textures" FILES ${${PROJECT_NAME}_TEXTURES} )

#========================================= SETUP LIBRARY =========================================#
# Enable debug logging on RELWITHDEBINFO configuration


# Set the startup project for visual studio
set_property(DIRECTORY ${CMAKE_CURRENT_LIST_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

if(DN_SHARED)
    set(CONFIG_TYPE SHARED)
else()
    set(CONFIG_TYPE STATIC)
endif()

set_source_files_properties(${${PROJECT_NAME}_IMGUIZMO_SRC} PROPERTIES 
    COMPILE_FLAGS "/Y-"
)

# Add the library
if (WIN32) 
    add_library(${PROJECT_NAME} STATIC
        ${GLOBAL_HEADER}
        ${GLOBAL_SRC}

        ${${PROJECT_NAME}_GLOBAL_HEADER} ${${PROJECT_NAME}_GLOBAL_SRC}
        ${${PROJECT_NAME}_WINDOWS_HEADER} ${${PROJECT_NAME}_WINDOWS_SRC}
        ${${PROJECT_NAME}_CORE_HEADER} ${${PROJECT_NAME}_CORE_SRC}
        ${${PROJECT_NAME}_EVENTS_HEADER} ${${PROJECT_NAME}_EVENTS_SRC}
        ${${PROJECT_NAME}_PROJECT_HEADER} ${${PROJECT_NAME}_PROJECT_SRC}
        ${${PROJECT_NAME}_RENDERER_HEADER} ${${PROJECT_NAME}_RENDERER_SRC}
        ${${PROJECT_NAME}_OPENGL_HEADER} ${${PROJECT_NAME}_OPENGL_SRC}
        ${${PROJECT_NAME}_IMGUI_HEADER} ${${PROJECT_NAME}_IMGUI_SRC}
        ${${PROJECT_NAME}_SCENE_HEADER} ${${PROJECT_NAME}_SCENE_SRC}
        ${${PROJECT_NAME}_UTILS_HEADER} ${${PROJECT_NAME}_UTILS_SRC}
        ${${PROJECT_NAME}_IMGUIZMO_HEADER} ${${PROJECT_NAME}_IMGUIZMO_SRC}
        ${${PROJECT_NAME}_MATH_HEADER} ${${PROJECT_NAME}_MATH_SRC}
        ${${PROJECT_NAME}_SCRIPT_ENGINE_HEADER} ${${PROJECT_NAME}_SCRIPT_ENGINE_SRC}
        ${${PROJECT_NAME}_UI_HEADER}
        ${${PROJECT_NAME}_PHYSICS_HEADER}

        ${${PROJECT_NAME}_GLM_HEADER}

        ${${PROJECT_NAME}_DEBUG_HEADER} ${${PROJECT_NAME}_DEBUG_SRC}

        ${${PROJECT_NAME}_STB_IMAGE_HEADER} ${${PROJECT_NAME}_STB_IMAGE_SRC}


        ${${PROJECT_NAME}_SHADERS}
        ${${PROJECT_NAME}_TEXTURES}
    )
elseif(UNIX)
    add_library(${PROJECT_NAME} 
        ${GLOBAL_HEADER}
        ${GLOBAL_SRC}

        ${${PROJECT_NAME}_GLOBAL_HEADER} ${${PROJECT_NAME}_GLOBAL_SRC}
        ${${PROJECT_NAME}_WINDOWS_HEADER} ${${PROJECT_NAME}_WINDOWS_SRC}
        ${${PROJECT_NAME}_CORE_HEADER} ${${PROJECT_NAME}_CORE_SRC}
        ${${PROJECT_NAME}_EVENTS_HEADER} ${${PROJECT_NAME}_EVENTS_SRC}
        ${${PROJECT_NAME}_RENDERER_HEADER} ${${PROJECT_NAME}_RENDERER_SRC}
        ${${PROJECT_NAME}_OPENGL_HEADER} ${${PROJECT_NAME}_OPENGL_SRC}
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
        target_compile_definitions(${PROJECT_NAME} PUBLIC
            DN_BUILD_SHARED
        )
    else()
        target_compile_definitions(${PROJECT_NAME} PUBLIC
            DN_BUILD_STATIC
        )
    endif()

    set_target_properties(${PROJECT_NAME} PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/../../bin
    )
endif()


target_compile_definitions(${PROJECT_NAME} PUBLIC
    __STDC_CONSTANT_MACROS
    _CRT_SRCURE_NO_WARNINGS
    DN_PLATFORM_WINDOWS
    DN_USE_GLFW
    DN_ENGINE
    DN_DEBUG
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
target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC ${SOURCES_DIR})

target_include_directories(${PROJECT_NAME} PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}/src
    ${CMAKE_CURRENT_LIST_DIR}/src/core
    ${CMAKE_CURRENT_LIST_DIR}/src/renderer
    ${CMAKE_CURRENT_LIST_DIR}/src/debug

    ${CMAKE_CURRENT_LIST_DIR}/dependencies/spdlog/include
    ${CMAKE_CURRENT_LIST_DIR}/dependencies/glfw/include
    ${CMAKE_CURRENT_LIST_DIR}/dependencies/glad/include
    ${CMAKE_CURRENT_LIST_DIR}/dependencies/glm
    ${CMAKE_CURRENT_LIST_DIR}/dependencies/imgui
    ${CMAKE_CURRENT_LIST_DIR}/dependencies/stb_image
    ${CMAKE_CURRENT_LIST_DIR}/dependencies/entt/include
    ${CMAKE_CURRENT_LIST_DIR}/dependencies/yaml-cpp/include
    ${CMAKE_CURRENT_LIST_DIR}/dependencies/yaml-cpp
    ${CMAKE_CURRENT_LIST_DIR}/dependencies/ImGuizmo
    ${CMAKE_CURRENT_LIST_DIR}/dependencies/Box2d/include
    ${CMAKE_CURRENT_LIST_DIR}/dependencies/mono/include
    ${CMAKE_CURRENT_LIST_DIR}/dependencies/filewatch
    $ENV{VULKAN_SDK}/Include

)

#========================================= LINKING LIBRARIES =========================================#
target_link_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/../../lib
    $<IF:$<CONFIG:Debug>,${CMAKE_CURRENT_LIST_DIR}/../../lib/mono/Debug,${CMAKE_CURRENT_LIST_DIR}/../../lib/mono/Release>
    # ${CMAKE_CURRENT_LIST_DIR}/../../lib/mono/Debug
    # ${CMAKE_CURRENT_LIST_DIR}/../../lib/mono/Release
    $ENV{VULKAN_SDK}/Lib
)

# target_link_libraries(${PROJECT_NAME} PUBLIC
#     user32.lib
#     opengl32.lib
#     dwmapi.lib
#     glfw3.lib
#     glad.lib
#     imgui.lib
#     yaml-cpp.lib

#     # vulkan-1.lib
#     # VkLayer_utils.lib


#     shaderc_shared.lib
#     spirv-cross-core.lib
#     spirv-cross-glsl.lib
# )

target_link_libraries(${PROJECT_NAME} PUBLIC
    user32.lib
    opengl32.lib
    dwmapi.lib
    glfw3.lib
    glad.lib
    imgui.lib
    yaml-cpp.lib

    Box2D.lib

    # vulkan-1.lib
    # VkLayer_utils.lib

    shaderc_sharedd.lib
    spirv-cross-cored.lib
    spirv-cross-glsld.lib

    libmono-static-sgen.lib

    Ws2_32.lib
    Winmm.lib
    Version.lib
    BCrypt.lib
    # $<IF:$<CONFIG:Debug>,mono-2.0-sgen.lib,Release>
)

set_property(TARGET ${PROJECT_NAME}
	PROPERTY
	MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")

foreach(type IN LISTS CMAKE_CONFIGURATION_TYPES)
    if(type)
        string(TOUPPER ${type} config)
        message("${PROJECT_NAME} config = ${config}")
        # set(CMAKE_CXX_FLAGS_${config} "${CMAKE_CXX_FLAGS_${config}} $<$<STREQUAL:${config},"Debug">:"/MTd">")
        # set(CMAKE_CXX_FLAGS_${config} "${CMAKE_CXX_FLAGS_${config}} -D${config}")
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
    # DEBUG_POSTFIX "d"
)

set(INSTALL_PATH 
${CMAKE_INSTALL_PREFIX}/${CMAKE_VS_PLATFORM_NAME})





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