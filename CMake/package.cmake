#
# Copyright (C) 2023, Advanced Micro Devices, Inc. All rights reserved.
#

# Packaging section

# Custom variables imported by this CMake stub which should be defined by parent CMake:
set(CPACK_PACKAGE_VERSION_RELEASE "${XRM_VERSION_RELEASE}")
set(CPACK_PACKAGE_VERSION_MAJOR "${XRM_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${XRM_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${XRM_VERSION_PATCH}")
set(CPACK_PACKAGE_NAME "xrm")

set(CPACK_PACKAGE_VERSION "${XRM_VERSION_STRING}")

find_program(UNAME uname)

execute_process(
    COMMAND awk -F= "$1==\"ID\" {print $2}" /etc/os-release
    COMMAND tr -d "\""
    COMMAND awk "{print tolower($1)}"
    OUTPUT_VARIABLE LINUX_FLAVOR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

if (${LINUX_FLAVOR} MATCHES "^centos")
    execute_process(
        COMMAND awk "{print $4}" /etc/redhat-release
        COMMAND tr -d "\""
        OUTPUT_VARIABLE CPACK_REL_VER
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
else()
    execute_process(
        COMMAND awk -F= "$1==\"VERSION_ID\" {print $2}" /etc/os-release
        COMMAND tr -d "\""
        OUTPUT_VARIABLE CPACK_REL_VER
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif()

execute_process(
    COMMAND ${UNAME} -r
    OUTPUT_VARIABLE LINUX_KERNEL_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
    COMMAND ${UNAME} -m
    OUTPUT_VARIABLE CPACK_ARCH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

if (${LINUX_FLAVOR} MATCHES "^(ubuntu|debian)")
    set(CPACK_GENERATOR "DEB")
    set(PACKAGE_KIND "DEB")
    # Modify the package name for the xrm component
    # Syntax is set(CPACK_<GENERATOR>_<COMPONENT>_PACKAGE_NAME "<name">)
    set(CPACK_DEBIAN_XRM_PACKAGE_NAME "xrm")

elseif (${LINUX_FLAVOR} MATCHES "^(rhel|centos|amzn|fedora|sles)")
    set(CPACK_GENERATOR "RPM")
    set(PACKAGE_KIND "RPM")
    # Modify the package name for the xrm component
    # Syntax is set(CPACK_<GENERATOR>_<COMPONENT>_PACKAGE_NAME "<name">)
    set(CPACK_RPM_XRM_PACKAGE_NAME "xrm")

endif()

set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}_${XRM_VERSION_RELEASE}.${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}_${LINUX_FLAVOR}${CPACK_REL_VER}-${CPACK_ARCH}")

message("-- ${CMAKE_BUILD_TYPE} ${PACKAGE_KIND} package")
set(CPACK_PACKAGE_CONTACT "XRM Development Team<xrm_dev@amd.com>")
set(CPACK_PACKAGE_VENDOR "AMD")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Xilinx FPGA Resource Manager (XRM)")
set(CPACK_PACKAGE_DESCRIPTION "The XRM is used to provide Xilinx FPGA Resource Manager")

set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_SOURCE_DIR}/CMake/config/prerm;${CMAKE_SOURCE_DIR}/CMake/config/postinst")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "xrt (>= 2.1.0),
                                  python3,
                                  libboost-serialization-dev (>= 1.58.0),
                                  libboost-system-dev (>= 1.58.0),
                                  libboost-filesystem-dev (>= 1.58.0),
                                  libboost-thread-dev (>= 1.58.0)")

SET(CPACK_RPM_POST_INSTALL_SCRIPT_FILE "${CMAKE_SOURCE_DIR}/CMake/config/postinst")
SET(CPACK_RPM_PRE_UNINSTALL_SCRIPT_FILE "${CMAKE_SOURCE_DIR}/CMake/config/prerm")
set(CPACK_RPM_PACKAGE_DEPENDS "xrt >= 2.1.0,
                               python3,
                               boost-serialization >= 1.58.0,
                               boost-system >= 1.58.0,
                               boost-filesystem >= 1.58.0,
                               boost-thread >= 1.58.0")
set(CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "/etc" "/etc/systemd" "/etc/systemd/system" "/opt" "/opt/xilinx" "/usr/lib" "/usr/lib/pkgconfig" "/usr/lib64/pkgconfig")
include(CPack)
