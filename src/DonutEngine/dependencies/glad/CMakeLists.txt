cmake_minimum_required(VERSION 3.22)
project(glad)

FILE(GLOB ${PROJECT_NAME}_HEADER 
    ${CMAKE_CURRENT_LIST_DIR}/include/glad/glad.h
    ${CMAKE_CURRENT_LIST_DIR}/include/KHR/khrplatform.h
)

FILE(GLOB ${PROJECT_NAME}_SRC
    ${CMAKE_CURRENT_LIST_DIR}/src/glad.c
)

add_library(${PROJECT_NAME} STATIC
    ${${PROJECT_NAME}_HEADER}
    ${${PROJECT_NAME}_SRC}
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include
)

target_compile_features(${PROJECT_NAME} PRIVATE
    cxx_std_17
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    #GLFW_INCLUDE_NONE
)

set_property(TARGET ${PROJECT_NAME}
	PROPERTY
	MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
)

if(CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(CONFIG_TYPES Debug Release ReleWithDebInfo MinSizeRel)
list(APPEND CONFIG_TYPE "")
foreach(type IN LISTS CONFIG_TYPES)
    set(config "")
    if(type)
        string(TOUPPER _${type} config)
    endif()

    set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY${config}   ${RUNTIME_DIR}
        LIBRARY_OUTPUT_DIRECTORY${config}   ${LIBRARY_DIR}
        ARCHIVE_OUTPUT_DIRECTORY${config}   ${LIBRARY_DIR}
        PDB_OUTPUT_DIRECTORY${config}       ${RUNTIME_DIR}
    )
endforeach()

set_target_properties(${PROJECT_NAME} PROPERTIES
	VS_DEBUG_WORKING_DIRECTORY $<IF:$<CONFIG:Debug>,Debug,Release>
    DEBUG_POSTFIX "d"
)

# install(TARGETS ${PROJECT_NAME}
#     EXPORT ${PROJECT_NAME}
#     RUNTIME DESTINATION bin
#     LIBRARY DESTINATION lib
# )

# set(version 1.0)
# set(CONFIG_VERSION_FILE ${CMAKE_CURRENT_LIST_DIR}/../../../../lib/conf/${PROJECT_NAME}ConfigVersion.cmake)
#config file
# install(EXPORT ${PROJECT_NAME} FILE ${PROJECT_NAME}Config.cmake 
#     DESTINATION ../../../../lib/config
# )

#版本文件
# include(CMakePackageConfigHelpers)
# write_basic_package_version_file(
#     ${CONFIG_VERSION_FILE}
#     VERSION ${version}
#     COMPATIBILITY SameMajorVersion  #版本兼容问题
# )

# install(FILES ${CONFIG_VERSION_FILE}
#     DESTINATION ../../../../lib/config/${PROJECT_NAME}-${version}
# )
