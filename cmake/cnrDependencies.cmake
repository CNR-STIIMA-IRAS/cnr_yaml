############################################
## It does takes inspiration from 
## Ton idea is to have a single file that can be included in the CMakeLists.txt of the package
## and in the >package>Config.cmake
## https://discourse.cmake.org/t/how-to-conditionally-call-either-find-pacakge-or-find-dependency/8175
if(${PROJECT_NAME} STREQUAL "cnr_yaml")
  message(STATUS "Loading cnrDependencies.cmake for the project '${PROJECT_NAME}'")
  macro(_find_package)
    find_package(${ARGN})
  endmacro()
else()
  message(STATUS "Running Project: ${PROJECT_NAME}. Loading cnrDependencies.cmake from cnr_yamlConfig.cmake")
  include(CMakeFindDependencyMacro)

  macro(_find_package)
    find_dependency(${ARGN})
  endmacro()
endif()
############################################


## Eigen
_find_package(Eigen3 REQUIRED COMPONENTS Core Dense  Geometry)

## yaml-cpp
_find_package(yaml-cpp REQUIRED)
if(yaml-cpp VERSION_LESS "0.8")
  set(YAML_CPP_HAS_NAMESPACE 0)
  _find_package(PkgConfig REQUIRED)
  pkg_check_modules(yaml-cpp_pkg_config REQUIRED yaml-cpp IMPORTED_TARGET)
else()
  set(YAML_CPP_HAS_NAMESPACE 1)
endif()

## Boost
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
_find_package(Boost REQUIRED COMPONENTS system filesystem program_options iostreams regex)

## Catkin is a fake dependency. It is used to test the cmake configuration
_find_package(catkin QUIET)

## Ament is a fake dependency. It is used to test the cmake configuration
_find_package(ament_cmake QUIET)

## If the file is included in CMakelists.txt, the following variables are defined
if(${PROJECT_NAME} STREQUAL "cnr_yaml")
  list(APPEND DEPENDENCIES_INCLUDE_DIRS "${EIGEN3_INCLUDE_DIRS}")
  list(APPEND DEPENDENCIES_INCLUDE_DIRS "${Boost_INCLUDE_DIRS}")
  if(NOT BOOL:${YAML_CPP_HAS_NAMESPACE})
    list(APPEND DEPENDENCIES_INCLUDE_DIRS  ${yaml-cpp_INCLUDE_DIRS})
  endif()

  list(APPEND DEPENDENCIES_LIBRARIES  Eigen3::Eigen)
  list(APPEND DEPENDENCIES_LIBRARIES  Boost::system  Boost::filesystem)

  if(BOOL:${YAML_CPP_HAS_NAMESPACE})
    list(APPEND DEPENDENCIES_LIBRARIES yaml-cpp::yaml-cpp)
  else()
    list(APPEND DEPENDENCIES_LIBRARIES yaml-cpp)
  endif()
endif()