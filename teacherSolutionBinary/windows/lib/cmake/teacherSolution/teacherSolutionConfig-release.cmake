#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "teacherSolution::teacherSolution" for configuration "Release"
set_property(TARGET teacherSolution::teacherSolution APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(teacherSolution::teacherSolution PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/libteacherSolution.dll"
  )

list(APPEND _cmake_import_check_targets teacherSolution::teacherSolution )
list(APPEND _cmake_import_check_files_for_teacherSolution::teacherSolution "${_IMPORT_PREFIX}/bin/libteacherSolution.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
