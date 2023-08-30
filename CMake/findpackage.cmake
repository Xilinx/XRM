#
# Copyright (C) 2023, Advanced Micro Devices, Inc. All rights reserved.
#

# Provides write_basic_package_version_file
include(CMakePackageConfigHelpers)

# Generate xrm-config.cmake
# For use by xrm consumers (using cmake) to import xrm libraries
set(PACKAGE_NAME "XRM")
configure_package_config_file (
  ${CMAKE_SOURCE_DIR}/CMake/config/xrm-config.in
  ${CMAKE_CURRENT_BINARY_DIR}/xrm-config.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_PREFIX}/xrm/share/cmake
  )

# Generate xrm-config-version.cmake
# Consumers my require a particular version
# This enables version checking
write_basic_package_version_file (
  ${CMAKE_CURRENT_BINARY_DIR}/xrm-config-version.cmake
  VERSION ${XRM_VERSION_STRING}
  COMPATIBILITY AnyNewerVersion
  )

# Install xrm-config.cmake and xrm-config-version.cmake
install (
  FILES ${CMAKE_CURRENT_BINARY_DIR}/xrm-config.cmake ${CMAKE_CURRENT_BINARY_DIR}/xrm-config-version.cmake
  DESTINATION ${CMAKE_INSTALL_PREFIX}/xrm/share/cmake
  )

# Generate and install xrm-targets.cmake
# This will generate a file that details all targets we have marked for export
# as part of the xrm-targets export group
# It will provide information such as the library file names and locations post install
install(
  EXPORT xrm-targets
  NAMESPACE ${PACKAGE_NAME}::
  DESTINATION ${CMAKE_INSTALL_PREFIX}/xrm/share/cmake
  )
