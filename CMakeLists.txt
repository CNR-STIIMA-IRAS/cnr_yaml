cmake_minimum_required(VERSION 3.16.0)

# ##############################################################################
# PRELIMINARIES                                                               ##
# ##############################################################################
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/cnrMacros.cmake)

get_project_name("package.xml" extracted_name extracted_version)

project(
  ${extracted_name}
  VERSION ${extracted_version}
  LANGUAGES CXX)

cnr_set_flags()
# ##############################################################################
# END - PRELIMINARIES                                                         ##
# ##############################################################################

# ##############################################################################
# OPTIONS                                                                     ##
# ##############################################################################
option(CMAKE_EXPORT_COMPILE_COMMANDS  "Export Compile Commands (clangd need it)"  ON)
option(BUILD_UNIT_TESTS               "Build the unit tests"                      ON)
option(BUILD_AS_A_CATKIN_PACKAGE      "Build the library using CATKIN tools"      OFF)
option(BUILD_AS_AN_AMENT_PACKAGE      "Build the library using AMENT tools"       OFF)

if(BUILD_AS_A_CATKIN_PACKAGE)
  if(BUILD_AS_AN_AMENT_PACKAGE)
    message(
      FATAL_ERROR
        "You selected both the options BUILD_AS_A_CATKIN_PACKAGE and BUILD_AS_AN_AMENT_PACKAGE.\
      Please check your environment and re-run configuration.")
  endif()
  if(NOT $ENV{ROS_VERSION} STREQUAL "1")
    message(FATAL_ERROR "You selected the option BUILD_AS_A_CATKIN_PACKAGE\
       but the env variable ROS_VERSION is '$ENV{ROS_VERSION}'")
  endif()
  find_package(catkin REQUIRED)
elseif(BUILD_AS_AN_AMENT_PACKAGE)
  if(NOT $ENV{ROS_VERSION} STREQUAL "2")
    message(FATAL_ERROR "You selected the option BUILD_AS_AN_AMENT_PACKAGE\
        but the env variable ROS_VERSION is '$ENV{ROS_VERSION}'")
  endif()
  find_package(ament_cmake REQUIRED)
endif()
# ##############################################################################
# END OPTIONS                                                                 ##
# ##############################################################################

# ##############################################################################
# DEPENDENCIES                                                                ##
# ##############################################################################
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/cnrDependencies.cmake)
# ##############################################################################
# END DEPENDANCIES                                                            ##
# ##############################################################################

# ##############################################################################
# Build                                                                       ##
# ##############################################################################
# SHARED
add_library(cnr_yaml SHARED
            ${CMAKE_CURRENT_SOURCE_DIR}/src/${PROJECT_NAME}/node_utils.cpp)

# Include DIRECTORIES: include and src
list(APPEND BUILD_INTERFACE_INCLUDE_DIRS "${DEPENDENCIES_INCLUDE_DIRS}"
     "${CMAKE_CURRENT_SOURCE_DIR}/include")

list(APPEND INSTALL_INTERFACE_INCLUDE_DIRS "${DEPENDENCIES_INCLUDE_DIRS}"
     include)
target_include_directories(
  cnr_yaml PUBLIC "$<BUILD_INTERFACE:${BUILD_INTERFACE_INCLUDE_DIRS}>"
                  "$<INSTALL_INTERFACE:${INSTALL_INTERFACE_INCLUDE_DIRS}>")

target_link_libraries(cnr_yaml PUBLIC "${DEPENDENCIES_TARGETS}")

set_target_properties(cnr_yaml PROPERTIES OUTPUT_NAME cnr_yaml
                                          CMAKE_POSITION_INDEPENDENT_CODE ON)

list(APPEND LIBRARIES_TARGETS_LIST cnr_yaml)
# ##############################################################################
# END - Build                                                                 ##
# ##############################################################################

# ##############################################################################
# TESTING                                                                     ##
# ##############################################################################
set(EXECUTABLE_TARGETS_LIST "")
if(BUILD_UNIT_TESTS)

  include(cmake/coverage.cmake)
  add_coverage_target("*/tests/*")

  add_executable(test_yaml ${CMAKE_CURRENT_SOURCE_DIR}/tests/test.cpp)

  cnr_configure_gtest(test_yaml cnr_yaml
                      ${CMAKE_CURRENT_SOURCE_DIR}/include include)

  target_compile_definitions(test_yaml
                             PRIVATE TEST_DIR="${CMAKE_CURRENT_LIST_DIR}")

  list(APPEND EXECUTABLE_TARGETS_LIST test_yaml)
endif()
# ##############################################################################
# TESTING                                                                     ##
# ##############################################################################

# ##############################################################################
# INSTALL                                                                     ##
# ##############################################################################
list(APPEND HEADERS_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/cnrInstall.cmake)
cnr_install("${LIBRARIES_TARGETS_LIST}" "${EXECUTABLE_TARGETS_LIST}"
            "${HEADERS_DIRS}")
# ##############################################################################
# END INSTALL                                                                 ##
# ##############################################################################
