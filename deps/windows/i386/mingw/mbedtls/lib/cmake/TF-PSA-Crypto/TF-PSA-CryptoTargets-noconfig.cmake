#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "TF-PSA-Crypto::tfpsacrypto" for configuration ""
set_property(TARGET TF-PSA-Crypto::tfpsacrypto APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(TF-PSA-Crypto::tfpsacrypto PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "C"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libtfpsacrypto.a"
  )

list(APPEND _cmake_import_check_targets TF-PSA-Crypto::tfpsacrypto )
list(APPEND _cmake_import_check_files_for_TF-PSA-Crypto::tfpsacrypto "${_IMPORT_PREFIX}/lib/libtfpsacrypto.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
