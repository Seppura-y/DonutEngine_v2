cmake_minimum_required(VERSION 3.22)

project(DonutEditor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(RUNTIME_DIR ${CMAKE_CURRENT_LIST_DIR}/../../bin)
set(LIBRARY_DIR ${CMAKE_CURRENT_LIST_DIR}/../../lib)

#===================================== CONFIGURATION TYPES OF THE PROJECT ==================================#

set(DN_SHARED ON CACHE BOOL "Build shared library?")


set(CONFIG_TYPE_DEBUG TRUE CACHE BOOL "Debug")
set(CONFIG_TYPE_RELEASE FALSE CACHE BOOL "Release")
set(CONFIG_TYPE_RELWITHDEBINFO FALSE CACHE BOOL "RelWithDebInfo")
set(CONFIG_TYPE_MINSIZEREL FALSE CACHE BOOL "MinSizeRel")

if(CONFIG_TYPE_DEBUG)
    list(APPEND CONFIG_TYPES "Debug;")
endif()

if(CONFIG_TYPE_RELEASE)
    list(APPEND CONFIG_TYPES "Release;")
endif()

if(CONFIG_TYPE_RELEASE)
    list(APPEND CONFIG_TYPES "RelWithDebInfo;")
endif()

if(CONFIG_TYPE_RELEASE)
    list(APPEND CONFIG_TYPES "MinSizeRel")
endif()

list(APPEND CONFIG_TYPES "Debug;")
list(APPEND CONFIG_TYPES "Release;")
list(APPEND CONFIG_TYPES "RelWithDebInfo;")
list(APPEND CONFIG_TYPES "MinSizeRel")

set(CMAKE_CONFIGURATION_TYPES ${CONFIG_TYPES})


set(CMAKE_INCLUDE_CURRENT_DIR ON)

#========================================= INCLUSION OF Project Files =========================================#

LIST(APPEND SOURCES_DIR 
    ${CMAKE_CURRENT_LIST_DIR}/src
    ${CMAKE_CURRENT_LIST_DIR}/src/config
    ${CMAKE_CURRENT_LIST_DIR}/src/media
    ${CMAKE_CURRENT_LIST_DIR}/src/handlers
    ${CMAKE_CURRENT_LIST_DIR}/src/log
    ${CMAKE_CURRENT_LIST_DIR}/src/video_renderer
    ${CMAKE_CURRENT_LIST_DIR}/src/media_manager
    ${CMAKE_CURRENT_LIST_DIR}/src/tools
)

include_directories(${SOURCE_DIR})

FILE(GLOB ${PROJECT_NAME}_SOURCES 
    ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp
)

FILE(GLOB ${PROJECT_NAME}_HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/src/*.h
)

FILE(GLOB ${PROJECT_NAME}_LOG_SOURCES 
    ${CMAKE_CURRENT_LIST_DIR}/src/log/*.cpp
)

FILE(GLOB ${PROJECT_NAME}_LOG_HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/src/log/*.h
)

FILE(GLOB ${PROJECT_NAME}_MEDIA_SOURCES 
    ${CMAKE_CURRENT_LIST_DIR}/src/media/*.cpp
)

FILE(GLOB ${PROJECT_NAME}_MEDIA_HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/src/media/*.h
)

FILE(GLOB ${PROJECT_NAME}_MEDIA_HANDLER_SOURCES 
    ${CMAKE_CURRENT_LIST_DIR}/src/handlers/*.cpp
)

FILE(GLOB ${PROJECT_NAME}_MEDIA_HANDLER_HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/src/handlers/*.h
)

FILE(GLOB ${PROJECT_NAME}_VIDEO_RENDERER_SOURCES 
    ${CMAKE_CURRENT_LIST_DIR}/src/video_renderer/*.cpp
)

FILE(GLOB ${PROJECT_NAME}_VIDEO_RENDERER_HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/src/video_renderer/*.h
)

FILE(GLOB ${PROJECT_NAME}_TOOLS_SOURCES 
    ${CMAKE_CURRENT_LIST_DIR}/src/tools/*.cpp
)

FILE(GLOB ${PROJECT_NAME}_TOOLS_HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/src/tools/*.h
)


FILE(GLOB ${PROJECT_NAME}_MEDIA_MANAGER_SOURCES 
    ${CMAKE_CURRENT_LIST_DIR}/src/media_manager/*.cpp
)

FILE(GLOB ${PROJECT_NAME}_MEDIA_MANAGER_HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/src/media_manager/*.h
)

source_group("Header Files//log" FILES ${${PROJECT_NAME}_LOG_HEADERS})
source_group("Source Files//log" FILES ${${PROJECT_NAME}_LOG_SOURCES})

source_group("Header Files//media" FILES ${${PROJECT_NAME}_MEDIA_HEADERS})
source_group("Source Files//media" FILES ${${PROJECT_NAME}_MEDIA_SOURCES})

source_group("Header Files//handler" FILES ${${PROJECT_NAME}_MEDIA_HANDLER_HEADERS})
source_group("Source Files//handler" FILES ${${PROJECT_NAME}_MEDIA_HANDLER_SOURCES})

source_group("Header Files//video_renderer" FILES ${${PROJECT_NAME}_RENDERER_HEADERS})
source_group("Source Files//video_renderer" FILES ${${PROJECT_NAME}_RENDERER_SOURCES})

source_group("Header Files//tools" FILES ${${PROJECT_NAME}_TOOLS_HEADERS})
source_group("Source Files//tools" FILES ${${PROJECT_NAME}_TOOLS_SOURCES})

source_group("Header Files//media_manager" FILES ${${PROJECT_NAME}_MEDIA_MANAGER_HEADERS})
source_group("Source Files//media_manager" FILES ${${PROJECT_NAME}_MEDIA_MANAGER_SOURCES})

#========================================= SETUP EXECTUABLE =========================================#


# Set the startup project for visual studio
set_property(DIRECTORY ${CMAKE_CURRENT_LIST_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

if(DN_SHARED)
    set(CONFIG_TYPE SHARED)
else()
    set(CONFIG_TYPE STATIC)
endif()

# Add the executable
if (WIN32) 
    add_executable(${PROJECT_NAME} WIN32
        ${${PROJECT_NAME}_HEADERS}
        ${${PROJECT_NAME}_SOURCES}

        ${${PROJECT_NAME}_LOG_HEADERS}
        ${${PROJECT_NAME}_LOG_SOURCES}

        ${${PROJECT_NAME}_MEDIA_HEADERS}
        ${${PROJECT_NAME}_MEDIA_SOURCES}

        ${${PROJECT_NAME}_MEDIA_HANDLER_HEADERS}
        ${${PROJECT_NAME}_MEDIA_HANDLER_SOURCES}

        ${${PROJECT_NAME}_TOOLS_HEADERS}
        ${${PROJECT_NAME}_TOOLS_SOURCES}

        ${${PROJECT_NAME}_MEDIA_MANAGER_HEADERS}
        ${${PROJECT_NAME}_MEDIA_MANAGER_SOURCES}
    )

elseif(UNIX)
    # add_executable(${PROJECT_NAME} ${SOURCES})
    # add_library(${name} ${CONFIG_TYPE} ${${name}_SRC} ${${name}_INTERFACE_HEADER} ${${name}_HEADER} ${QT_FILES})
endif()

# set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "frontend")

set(PUB_HEADERS         
    ${CMAKE_CURRENT_LIST_DIR}/src/media_types.h
    ${CMAKE_CURRENT_LIST_DIR}/src/core.h
    ${${PROJECT_NAME}_LOG_HEADERS}
    ${${PROJECT_NAME}_MEDIA_HANDLER_HEADERS}
    ${${PROJECT_NAME}_MEDIA_HEADERS}
    ${${PROJECT_NAME}_RENDERER_HEADERS}
    ${${PROJECT_NAME}_TOOLS_HEADERS}
)
set_target_properties(${PROJECT_NAME} PROPERTIES 
    PUBLIC_HEADER "${PUB_HEADERS}"
    # VERSION ${PROJECT_VERSION}
)

if( MSVC )
    SET( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:CONSOLE /ENTRY:mainCRTStartup" )

    set_target_properties(${PROJECT_NAME} PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/../../bin
    )
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE
    __STDC_CONSTANT_MACROS
    DN_PLATFORM_WINDOWS
    DN_BUILD_SHARED
)

target_compile_features(${PROJECT_NAME} PRIVATE
    cxx_std_17
)

# Add the target includes for the project 
# target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../../include/ffmpeg)
# target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../../include/spdlog/include)
# target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../../include)
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${SOURCES_DIR})

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_LIST_DIR}/../../dependencies/
    ${CMAKE_CURRENT_LIST_DIR}/../../include
    ${CMAKE_CURRENT_LIST_DIR}/../../include/spdlog/include
    ${CMAKE_CURRENT_LIST_DIR}/../DonutEngine/src
)

#========================================= LINKING LIBRARIES =========================================#
target_link_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../../lib
)

target_link_libraries(${PROJECT_NAME} 
    user32.lib
    dwmapi.lib
    DonutEngined
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

set(INSTALL_PATH 
${CMAKE_INSTALL_PREFIX}/${CMAKE_VS_PLATFORM_NAME}/)


install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}
    RUNTIME         DESTINATION bin
    ARCHIVE         DESTINATION lib
    LIBRARY         DESTINATION lib
)

install(EXPORT ${PROJECT_NAME} FILE "${PROJECT_NAME}Config.cmake" DESTINATION config)

install(DIRECTORY 
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
    DESTINATION
    ${INSTALL_PATH}
)