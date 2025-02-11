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
option(CMAKE_EXPORT_COMPILE_COMMANDS "Export Compile Commands (clangd need it)" ON)
option(BUILD_UNIT_TESTS "Build the unit tests" ON)
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
# ##############################################################################
# END DEPENDANCIES                                                            ##
# ##############################################################################

# ##############################################################################
# Build                                                                       ##
# ##############################################################################
add_library(cnr_yaml SHARED
  ${CMAKE_CURRENT_SOURCE_DIR}/src/${PROJECT_NAME}/node_utils.cpp)

target_include_directories(
  cnr_yaml PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)

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
# CONFIGURE THE PACKAGE                                                       ##
# ##############################################################################
set(CNR_PACKAGE_LIB_DESTINATION         "lib")
set(CNR_PACKAGE_BIN_DESTINATION         "bin")
set(CNR_PACKAGE_INCLUDE_DESTINATION     "include")
set(CNR_PACKAGE_CONFIG_DESTINATION      "share/${PROJECT_NAME}/cmake")

cnr_cmake_package_file("${LIBRARIES_TARGETS_LIST}" "${EXECUTABLE_TARGETS_LIST}")
# ##############################################################################
# END - CONFIGURE THE PACKAGE                                                 ##
# ##############################################################################

# ##############################################################################
# INSTALL THE PACKAGE                                                         ##
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
