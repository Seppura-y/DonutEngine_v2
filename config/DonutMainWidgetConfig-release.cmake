#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "DonutMainWidget" for configuration "Release"
set_property(TARGET DonutMainWidget APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(DonutMainWidget PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/DonutMainWidget.exe"
  )

list(APPEND _IMPORT_CHECK_TARGETS DonutMainWidget )
list(APPEND _IMPORT_CHECK_FILES_FOR_DonutMainWidget "${_IMPORT_PREFIX}/bin/DonutMainWidget.exe" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
