cmake_minimum_required(VERSION 2.8.11)

project(xrmd)
set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:/usr/lib/pkgconfig")

# Default component name for any install() command without the COMPONENT argument
set(CMAKE_INSTALL_DEFAULT_COMPONENT_NAME "xrm")

# Version
set(XRM_VERSION_RELEASE 202220)
set(XRM_VERSION_MAJOR 1)
set(XRM_VERSION_MINOR 5)

if (DEFINED ENV{XRM_VERSION_PATCH})
  set(XRM_VERSION_PATCH $ENV{XRM_VERSION_PATCH})
else(DEFINED $ENV{XRM_VERSION_PATCH})
  set(XRM_VERSION_PATCH 0)
endif(DEFINED ENV{XRM_VERSION_PATCH})

set(XRM_SOVERSION ${XRM_VERSION_MAJOR})
set(XRM_VERSION_STRING ${XRM_VERSION_MAJOR}.${XRM_VERSION_MINOR}.${XRM_VERSION_PATCH})

# Linking variables
find_package(PkgConfig REQUIRED)
find_package(XRT PATHS /opt/xilinx/xrt)
find_library(UUID_LIBRARIES NAMES uuid REQUIRED)

# Try pkg-config for older XRT
if (NOT ${XRT_FOUND})
  pkg_check_modules(XRT REQUIRED xrt)
  set(XRT_CORE_LIBRARIES ${XRT_LDFLAGS})
endif()

if (DEFINED ENV{XRM_BOOST_INSTALL})
  set(Boost_USE_STATIC_LIBS ON)
  find_package(Boost 
      HINTS $ENV{XRM_BOOST_INSTALL}
      COMPONENTS system filesystem thread serialization REQUIRED)
else()
  find_package(Boost COMPONENTS system filesystem thread serialization REQUIRED)
endif()

set (CMAKE_CXX_STANDARD 14)
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMake/")
file(GLOB SOURCES "src/daemon/*.cpp")

# Bring the headers into the project
include_directories(${XRT_INCLUDE_DIRS}
                     "src/lib"
                     "src/daemon")
# Generate the executable
add_executable(${PROJECT_NAME} ${SOURCES} ${CONFIG_FILE} ${SERVICE_FILE})
target_compile_options(${PROJECT_NAME} PRIVATE -Werror -Wall -Wextra)
target_link_libraries(${PROJECT_NAME}
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_THREAD_LIBRARY}
  ${Boost_SERIALIZATION_LIBRARY}
  ${XRT_CORE_LIBRARIES}
  ${UUID_LIBRARIES}
  ${CMAKE_DL_LIBS}
  ${CMAKE_THREAD_LIBS_INIT}
)

file(GLOB SOURCE_LIBS "src/lib/*.cpp")
add_library("xrm" SHARED ${SOURCE_LIBS})
target_link_libraries("xrm"
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_THREAD_LIBRARY}
  ${Boost_SERIALIZATION_LIBRARY}
  ${UUID_LIBRARIES}
  ${CMAKE_DL_LIBS}
)

set_target_properties("xrm" PROPERTIES VERSION ${XRM_VERSION_STRING}
  SOVERSION ${XRM_SOVERSION})

file(GLOB SOURCE_PLUGIN "src/plugin/*.cpp")
add_library("xrmpluginexample" SHARED ${SOURCE_PLUGIN})
target_link_libraries("xrmpluginexample"
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_THREAD_LIBRARY}
  ${Boost_SERIALIZATION_LIBRARY}
  ${UUID_LIBRARIES}
)

# Set the location for library installation
install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/xrm/bin)
install(TARGETS "xrm" DESTINATION ${CMAKE_INSTALL_PREFIX}/xrm/lib EXPORT xrm-targets)
install(TARGETS "xrmpluginexample" DESTINATION ${CMAKE_INSTALL_PREFIX}/xrm/plugin)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/src/lib/
        DESTINATION ${CMAKE_INSTALL_PREFIX}/xrm/include
        FILES_MATCHING PATTERN "*.h")
install(DIRECTORY ${CMAKE_SOURCE_DIR}/src/lib/
        DESTINATION ${CMAKE_INSTALL_PREFIX}/xrm/include
        FILES_MATCHING PATTERN "*.hpp")
install(DIRECTORY ${CMAKE_SOURCE_DIR}/src/lib/
        DESTINATION ${CMAKE_INSTALL_PREFIX}/xrm/include
        FILES_MATCHING PATTERN "*.py")
install(PROGRAMS ${CMAKE_SOURCE_DIR}/src/cli/xrmadm
        DESTINATION ${CMAKE_INSTALL_PREFIX}/xrm/bin)
install(FILES ${CMAKE_SOURCE_DIR}/xrm.ini
        DESTINATION ${CMAKE_INSTALL_PREFIX}/xrm/bin)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/test
        DESTINATION ${CMAKE_INSTALL_PREFIX}/xrm)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/tools
        DESTINATION ${CMAKE_INSTALL_PREFIX}/xrm
        FILE_PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)
install(FILES ${CMAKE_SOURCE_DIR}/setup.sh
        DESTINATION ${CMAKE_INSTALL_PREFIX}/xrm)
install(FILES ${CMAKE_SOURCE_DIR}/setup.csh
        DESTINATION ${CMAKE_INSTALL_PREFIX}/xrm)
install(FILES ${CMAKE_SOURCE_DIR}/version.json
        DESTINATION ${CMAKE_INSTALL_PREFIX}/xrm)

include(CMake/version.cmake)
include(CMake/service.cmake)
include(CMake/package.cmake)
include(CMake/pkgconfig.cmake)
include(CMake/findpackage.cmake)
