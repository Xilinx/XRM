configure_file(
    ${CMAKE_SOURCE_DIR}/CMake/config/xrmd.service.in
    ${CMAKE_SOURCE_DIR}/CMake/config/xrmd.service)
set(SERVICE_FILE "CMake/config/xrmd.service")

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(NOT DEFINED ENV{CONDA_DEFAULT_ENV})
      install(FILES ${SERVICE_FILE} DESTINATION "/etc/systemd/system" )
    endif()
endif(CMAKE_BUILD_TYPE STREQUAL "Release")
