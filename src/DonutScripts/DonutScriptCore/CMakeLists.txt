cmake_minimum_required(VERSION 3.22)
project("DonutScriptCore" CSharp)

list(APPEND CONFIG_TYPES "Debug;")
list(APPEND CONFIG_TYPES "Release;")
list(APPEND CONFIG_TYPES "RelWithDebInfo;")
list(APPEND CONFIG_TYPES "MinSizeRel")

set(CMAKE_CONFIGURATION_TYPES ${CONFIG_TYPES})

set(RUNTIME_DIR ${CMAKE_CURRENT_LIST_DIR}/../../DonutEditor/Resources/Scripts/)
# set(LIBRARY_DIR ${CMAKE_CURRENT_LIST_DIR}/../../lib)

add_library(${PROJECT_NAME} SHARED
    # src/Player.cs
    # src/Camera.cs

    src/Donut/InternalCalls.cs
    src/Donut/KeyCode.cs
    src/Donut/Input.cs
    src/Donut/Vector2.cs
    src/Donut/Vector3.cs
    
    src/Donut/Scene/Entity.cs
    src/Donut/Scene/Components.cs
)

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

set_property(TARGET ${PROJECT_NAME} PROPERTY DOTNET_TARGET_FRAMEWORK_VERSION "v4.7.2")
set_property(TARGET ${PROJECT_NAME} PROPERTY WIN32_EXECUTABLE FALSE)
set_property(TARGET ${PROJECT_NAME} PROPERTY VS_CONFIGURATION_TYPE ClassLibrary)
set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DOTNET_REFERENCES
        "Microsoft.CSharp"
        "PresentationCore"
        "PresentationFramework"
        "System"
        "System.Core"
        "System.Data"
        "System.Data.DataSetExtensions"
        "System.Windows.Forms"
        "System.Net.Http"
        "System.Xaml"
        "System.Xml"
        "System.Xml.Linq"
        "WindowsBase"
)