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
## Eigen
find_package(Eigen3 REQUIRED COMPONENTS Core Dense  Geometry)
list(APPEND DEPENDENCIES_LIBRARIES  Eigen3::Eigen)
list(APPEND DEPENDENCIES_INCLUDE_DIRS "${EIGEN3_INCLUDE_DIRS}")

## yaml-cpp
find_package(yaml-cpp REQUIRED)
if(yaml-cpp VERSION_LESS "0.8")
  set(YAML_CPP_HAS_NAMESPACE 0)
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(yaml-cpp_pkg_config REQUIRED yaml-cpp IMPORTED_TARGET)
else()
  set(YAML_CPP_HAS_NAMESPACE 1)
endif()
if(BOOL:${YAML_CPP_HAS_NAMESPACE})
  list(APPEND DEPENDENCIES_LIBRARIES yaml-cpp::yaml-cpp)
else()
  list(APPEND DEPENDENCIES_LIBRARIES yaml-cpp)
endif()

## Boost
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost REQUIRED COMPONENTS system filesystem program_options iostreams regex)
list(APPEND DEPENDENCIES_LIBRARIES  Boost::system  Boost::filesystem)
list(APPEND DEPENDENCIES_INCLUDE_DIRS "${Boost_INCLUDE_DIRS}")

if(NOT BOOL:${YAML_CPP_HAS_NAMESPACE})
  list(APPEND DEPENDENCIES_INCLUDE_DIRS  ${yaml-cpp_INCLUDE_DIRS})
endif()

## Catkin is a fake dependency. It is used to test the cmake configuration
find_package(catkin QUIET)
if(${catkin_FOUND})
  set(PACKAGE_LIB_DESTINATION     "${CATKIN_GLOBAL_LIB_DESTINATION}")
  set(PACKAGE_BIN_DESTINATION     "${CATKIN_GLOBAL_LIBEXEC_DESTINATION}/${PROJECT_NAME}")
  set(PACKAGE_INCLUDE_DESTINATION "${CATKIN_GLOBAL_INCLUDE_DESTINATION}/${PROJECT_NAME}")
  set(CONFIG_INSTALL_DIR          "share/${PROJECT_NAME}/cmake_alternative")
else()
  set(PACKAGE_LIB_DESTINATION      "${CMAKE_INSTALL_PREFIX}/lib")
  set(PACKAGE_BIN_DESTINATION      "${CMAKE_INSTALL_PREFIX}/bin")
  set(PACKAGE_INCLUDE_DESTINATION  "${CMAKE_INSTALL_PREFIX}/include")
  set(CONFIG_INSTALL_DIR           "share/${PROJECT_NAME}/cmake")
endif()
# ##############################################################################
# END DEPENDANCIES        ##
# ##############################################################################



# ##############################################################################
# Build       ##
# ##############################################################################

## SHARED
add_library(cnr_yaml SHARED ${CMAKE_CURRENT_SOURCE_DIR}/src/${PROJECT_NAME}/node_utils.cpp)

target_include_directories(
  cnr_yaml
  PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>"
  "$<INSTALL_INTERFACE:include>")

target_link_libraries(
  cnr_yaml
  PUBLIC "${DEPENDENCIES_LIBRARIES}")

set_target_properties(cnr_yaml PROPERTIES 
  OUTPUT_NAME cnr_yaml
  CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(cnr_yaml::cnr_yaml ALIAS cnr_yaml)
list(APPEND LIBRARIES_TARGETS_LIST cnr_yaml)

## STATIC
add_library(cnr_yaml_static STATIC ${CMAKE_CURRENT_SOURCE_DIR}/src/${PROJECT_NAME}/node_utils.cpp)
target_include_directories(
    cnr_yaml_static
    PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>"
    "$<INSTALL_INTERFACE:include>")
  
target_link_libraries(
    cnr_yaml_static
    PUBLIC "${DEPENDENCIES_LIBRARIES}")

set_target_properties(cnr_yaml_static PROPERTIES 
  OUTPUT_NAME cnr_yaml
  CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(cnr_yaml::cnr_yaml_static ALIAS cnr_yaml_static)
list(APPEND LIBRARIES_TARGETS_LIST cnr_yaml_static)
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
list(APPEND HEADERS_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include)

cnr_install(
  "${LIBRARIES_TARGETS_LIST}"
  "${EXECUTABLE_TARGETS_LIST}"
  "${HEADERS_DIRS}")
# ##############################################################################
# END INSTALL     ##
# ##############################################################################
