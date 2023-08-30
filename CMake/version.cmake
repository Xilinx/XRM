#
# Copyright (C) 2023, Advanced Micro Devices, Inc. All rights reserved.
#

if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
    execute_process(
        COMMAND git rev-parse --abbrev-ref HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE XRM_GIT_BRANCH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    execute_process(
        COMMAND git rev-parse --verify HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE XRM_GIT_COMMIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    execute_process(
        COMMAND git log -1 --pretty=format:%cD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE XRM_GIT_COMMIT_DATE
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
else(EXISTS "${CMAKE_SOURCE_DIR}/.git")
    set(XRM_GIT_BRANCH "")
    set(XRM_GIT_COMMIT_HASH "")
    set(XRM_GIT_COMMIT_DATE "")
endif(EXISTS "${CMAKE_SOURCE_DIR}/.git")

configure_file(
    ${CMAKE_SOURCE_DIR}/CMake/config/xrm_version.h.in
    ${CMAKE_SOURCE_DIR}/src/lib/xrm_version.h
)

configure_file(
    ${CMAKE_SOURCE_DIR}/CMake/config/version.json.in
    ${CMAKE_SOURCE_DIR}/version.json
)
