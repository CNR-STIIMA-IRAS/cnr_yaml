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
option(CMAKE_EXPORT_COMPILE_COMMANDS "Export Compile Commands (clangd need it)"
       ON)
option(BUILD_UNIT_TESTS "Build the unit tests" ON)
option(BUILD_AS_A_CATKIN_PACKAGE "Build the library using CATKIN tools" OFF)
option(BUILD_AS_AN_AMENT_PACKAGE "Build the library using AMENT tools" OFF)

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

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}-compile-options.cmake.in"
               "${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}-compile-options.cmake" @ONLY)
# ##############################################################################
# END OPTIONS                                                                 ##
# ##############################################################################

# ##############################################################################
# DEPENDENCIES                                                                ##
# ##############################################################################
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}-dependencies.cmake)

list(APPEND
  DEPENDENCIES_TARGETS 
  Eigen3::Eigen 
  yaml-cpp::yaml-cpp
  Boost::system
  Boost::filesystem
  Boost::program_options
  Boost::iostreams
  Boost::regex
)

list(APPEND 
  DEPENDENCIES_INCLUDE_DIRS 
  ${EIGEN3_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
)
list(APPEND
  DEPENDENCIES_LINK_LIBRARIES
  ${Boost_FILESYSTEM_LIBRARY_RELEASE}
  ${Boost_SYSTEM_LIBRARY_RELEASE}
  ${Boost_PROGRAM_OPTIONS_LIBRARY_RELEASE}
  ${Boost_IOSTREAMS_LIBRARY_RELEASE}
  ${Boost_REGEX_LIBRARY_RELEASE}
)

# NOTE: This is not for dependency, but for the configuration...
#       however, this macro must be called before the creation of
#       the targets
if(BUILD_AS_A_CATKIN_PACKAGE)
  catkin_package(
    INCLUDE_DIRS include
    LIBRARIES cnr_yaml
    DEPENDS Boost YAML_CPP EIGEN3
    CFG_EXTRAS ${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}-compile-options.cmake)
endif()
# ##############################################################################
# END DEPENDANCIES                                                            ##
# ##############################################################################

# ##############################################################################
# Build                                                                       ##
# ##############################################################################
add_library(cnr_yaml SHARED
            ${CMAKE_CURRENT_SOURCE_DIR}/src/${PROJECT_NAME}/node_utils.cpp)

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

add_library(cnr_yaml::cnr_yaml ALIAS cnr_yaml)
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

  cnr_configure_gtest(test_yaml cnr_yaml ${CMAKE_CURRENT_SOURCE_DIR}/include
                      include)

  target_compile_definitions(test_yaml
                             PRIVATE TEST_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

  list(APPEND EXECUTABLE_TARGETS_LIST test_yaml)
endif()
# ##############################################################################
# TESTING                                                                     ##
# ##############################################################################

# ##############################################################################
# CONFIGURE AND INSTALL DESTINATIONS                                          ##
# ##############################################################################
if(BUILD_AS_A_CATKIN_PACKAGE)
  set(CNR_PACKAGE_LIB_DESTINATION     "${CATKIN_GLOBAL_LIB_DESTINATION}")
  set(CNR_PACKAGE_BIN_DESTINATION     "${CATKIN_GLOBAL_LIB_DESTINATION}/${PROJECT_NAME}")
  set(CNR_PACKAGE_INCLUDE_DESTINATION "${CATKIN_GLOBAL_INCLUDE_DESTINATION}")
  set(CNR_PACKAGE_CONFIG_DESTINATION  "${CATKIN_GLOBAL_SHARE_DESTINATION}/${PROJECT_NAME}")
else()
  set(CNR_PACKAGE_LIB_DESTINATION "lib")
  set(CNR_PACKAGE_BIN_DESTINATION "bin")
  set(CNR_PACKAGE_INCLUDE_DESTINATION "include")
  set(CNR_PACKAGE_CONFIG_DESTINATION "share/${PROJECT_NAME}/cmake")
endif()
# ##############################################################################
# END - CONFIGURE AND INSTALL DESTINATIONS                                    ##
# ##############################################################################

# ##############################################################################
# CONFIGURE THE PACKAGE                                           ##
# ##############################################################################
if(NOT BUILD_AS_A_CATKIN_PACKAGE)
  cnr_cmake_package_file("${LIBRARIES_TARGETS_LIST}"
                         "${EXECUTABLE_TARGETS_LIST}")
endif()
# ##############################################################################
# END - CONFIGURE THE PACKAGE                                           ##
# ##############################################################################

# ##############################################################################
# INSTALL THE PACKAGE                                           ##
# ##############################################################################
install(
  DIRECTORY include/${PROJECT_NAME}/
  DESTINATION ${CNR_PACKAGE_INCLUDE_DESTINATION}/${PROJECT_NAME}
  FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
  PATTERN ".svn" EXCLUDE)

install(
  TARGETS "${LIBRARIES_TARGETS_LIST}" "${EXECUTABLE_TARGETS_LIST}"
  EXPORT "${TARGETS_EXPORT_NAME}"
  ARCHIVE DESTINATION ${CNR_LIB_DESTINATION}
  LIBRARY DESTINATION ${CNR_LIB_DESTINATION}
  RUNTIME DESTINATION ${CNR_BIN_DESTINATION})

if(NOT BUILD_AS_A_CATKIN_PACKAGE)
  # ------------------------------------------------------------------------------
  # Install cmake targets files and also namespaced alias targets
  install(
    EXPORT "${TARGETS_EXPORT_NAME}"
    NAMESPACE "${CONFIG_NAMESPACE}"
    DESTINATION "${CNR_PACKAGE_CONFIG_DESTINATION}"
    FILE ${TARGETS_EXPORT_NAME}.cmake)

  # ------------------------------------------------------------------------------
  # Install cmake config files
  install(FILES "${PROJECT_CONFIG_OUTPUT}" 
                "${VERSION_CONFIG}"
                "${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}-dependencies.cmake"
          DESTINATION "${CNR_PACKAGE_CONFIG_DESTINATION}")
endif()
# ##############################################################################
# END INSTALL                                                                 ##
# ##############################################################################
