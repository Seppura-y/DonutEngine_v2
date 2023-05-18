cmake_minimum_required(VERSION 3.22)

project(DonutEditor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(RUNTIME_DIR ${CMAKE_CURRENT_LIST_DIR}/../../bin)
set(LIBRARY_DIR ${CMAKE_CURRENT_LIST_DIR}/../../lib)

#===================================== CONFIGURATION TYPES OF THE PROJECT ==================================#

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
)

include_directories(${SOURCE_DIR})

FILE(GLOB ${PROJECT_NAME}_SOURCES 
    ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp
)

FILE(GLOB ${PROJECT_NAME}_HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/src/*.h
)

#========================================= SETUP EXECTUABLE =========================================#

# Add the executable
if (WIN32) 
    add_executable(${PROJECT_NAME} WIN32
        ${${PROJECT_NAME}_HEADERS}
        ${${PROJECT_NAME}_SOURCES}
    )

elseif(UNIX)
    # add_executable(${PROJECT_NAME} ${SOURCES})
    # add_library(${name} ${CONFIG_TYPE} ${${name}_SRC} ${${name}_INTERFACE_HEADER} ${${name}_HEADER} ${QT_FILES})
endif()

# set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "frontend")


if( MSVC )
    SET( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:CONSOLE /ENTRY:mainCRTStartup" )

    set_target_properties(${PROJECT_NAME} PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/../../bin
    )
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE
    __STDC_CONSTANT_MACROS
    DN_PLATFORM_WINDOWS
)

target_compile_features(${PROJECT_NAME} PRIVATE
    cxx_std_17
)

# Add the target includes for the project 

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_LIST_DIR}/../DonutEngine/src
    ${CMAKE_CURRENT_LIST_DIR}/../DonutEngine/dependencies
    ${CMAKE_CURRENT_LIST_DIR}/../DonutEngine/dependencies/spdlog/include
    ${CMAKE_CURRENT_LIST_DIR}/../DonutEngine/dependencies/glm
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