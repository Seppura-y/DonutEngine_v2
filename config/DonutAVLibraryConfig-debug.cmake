#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "DonutAVLibrary" for configuration "Debug"
set_property(TARGET DonutAVLibrary APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(DonutAVLibrary PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/DonutAVLibrary.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS DonutAVLibrary )
list(APPEND _IMPORT_CHECK_FILES_FOR_DonutAVLibrary "${_IMPORT_PREFIX}/lib/DonutAVLibrary.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
