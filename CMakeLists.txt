cmake_minimum_required(VERSION 3.16.0)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/cnrConfigMacros.cmake)

# ##############################################################################
# PRELIMINARIES           ##
# ##############################################################################
get_project_name("package.xml" extracted_name extracted_version)

project(
  ${extracted_name}
  VERSION ${extracted_version}
  LANGUAGES CXX)

cnr_set_flags()
# ##############################################################################
# END - PRELIMINARIES     ##
# ##############################################################################



# ##############################################################################
# DEPENDENCIES            ##
# ##############################################################################
find_package(Eigen3 REQUIRED COMPONENTS Core Dense  Geometry)

find_package(yaml-cpp REQUIRED)
if(yaml-cpp VERSION_LESS "0.8")
  set(YAML_CPP_HAS_NAMESPACE 0)
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(yaml-cpp_pkg_config REQUIRED yaml-cpp IMPORTED_TARGET)
else()
  set(YAML_CPP_HAS_NAMESPACE 1)
endif()

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost REQUIRED COMPONENTS system filesystem program_options iostreams regex)

list(APPEND DEPENDENCIES_INCLUDE_DIRS "${Boost_INCLUDE_DIRS}" "${EIGEN3_INCLUDE_DIRS}")

if(NOT BOOL:${YAML_CPP_HAS_NAMESPACE})
  list(APPEND DEPENDENCIES_INCLUDE_DIRS  ${yaml-cpp_INCLUDE_DIRS})
endif()

if(BOOL:${YAML_CPP_HAS_NAMESPACE})
  list(APPEND DEPENDENCIES_ROS_LIBRARIES yaml-cpp::yaml-cpp)
else()
  list(APPEND DEPENDENCIES_ROS_LIBRARIES yaml-cpp)
endif()


cnr_install_directories(CNR_INSTALL_INCLUDE_DIR CNR_INSTALL_LIB_DIR
                        CNR_INSTALL_BIN_DIR CNR_INSTALL_SHARE_DIR)
# ##############################################################################
# END DEPENDANCIES        ##
# ##############################################################################



# ##############################################################################
# Build       ##
# ##############################################################################
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/${PROJECT_NAME}/)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/)

add_library(cnr_yaml SHARED ${SRC_DIR}/node_utils.cpp)

target_include_directories(
  cnr_yaml
  PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>"
  "$<INSTALL_INTERFACE:include>")

target_link_libraries(
  cnr_yaml
  PUBLIC $<${YAML_CPP_HAS_NAMESPACE}:yaml-cpp::yaml-cpp>
  PUBLIC $<$<NOT:${YAML_CPP_HAS_NAMESPACE}>:PkgConfig::yaml-cpp_pkg_config>
  PUBLIC Boost::system
  PUBLIC Boost::filesystem
  PUBLIC Eigen3::Eigen)

add_library(cnr_yaml::cnr_yaml ALIAS cnr_yaml)
list(APPEND LIBRARIES_TARGETS_LIST cnr_yaml)
# ##############################################################################
# END - Build ##
# ##############################################################################



# ##############################################################################
# TESTING         ##
# ##############################################################################
set(EXECUTABLE_TARGETS_LIST "")
if(ENABLE_TESTING)
  
  include(cmake/coverage.cmake)
  add_coverage_target("*/tests/*")

  find_package(GTest REQUIRED)
  include(GoogleTest)
  enable_testing()
  include(CTest REQUIRED)

  add_executable(test_yaml
                ${CMAKE_CURRENT_SOURCE_DIR}/tests/test.cpp)

  cnr_configure_gtest(test_yaml cnr_yaml::cnr_yaml
                      ${CMAKE_CURRENT_SOURCE_DIR}/include include)

  target_compile_definitions(test_yaml
                      PRIVATE TEST_DIR="${CMAKE_CURRENT_LIST_DIR}")

  list(APPEND EXECUTABLE_TARGETS_LIST test_yaml)
endif()
# ##############################################################################
# TESTING         ##
# ##############################################################################



# ##############################################################################
# INSTALL         ##
# ##############################################################################
list(APPEND HEADERS_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
     "${CMAKE_CURRENT_BINARY_DIR}/include/cnr_yaml")

if(USE_ROS1)
  cnr_install_catkin_policy()
else()
  set(PROJECT_VERSION 1.0.0)
  cnr_install_cmake_policy(
    ${CNR_INSTALL_INCLUDE_DIR}
    ${CNR_INSTALL_LIB_DIR}
    ${CNR_INSTALL_BIN_DIR}
    ${extracted_version}
    "${LIBRARIES_TARGETS_LIST}"
    "${EXECUTABLE_TARGETS_LIST}"
    "${HEADERS_DIRS}")
endif()
# ##############################################################################
# END INSTALL     ##
# ##############################################################################

