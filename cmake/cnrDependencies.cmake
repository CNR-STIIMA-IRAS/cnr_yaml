############################################
## It does takes inspiration from 
## Ton idea is to have a single file that can be included in the CMakeLists.txt of the package
## and in the >package>Config.cmake
## https://discourse.cmake.org/t/how-to-conditionally-call-either-find-pacakge-or-find-dependency/8175
include(CMakeFindDependencyMacro)

if(${PROJECT_NAME} STREQUAL "cnr_yaml")
  message(STATUS "Loading cnr_yamlDependencies.cmake for the project '${PROJECT_NAME}'")
  macro(_find_package)
    find_package(${ARGN})
  endmacro()
else()
  message(STATUS "Running Project: ${PROJECT_NAME}. Loading cnr_yamlDependencies.cmake from cnr_yamlConfig.cmake")
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
  _find_package(PkgConfig REQUIRED)
  pkg_check_modules(YAML_CPP REQUIRED yaml-cpp IMPORTED_TARGET GLOBAL)
  add_library(yaml-cpp::yaml-cpp ALIAS PkgConfig::YAML_CPP)
endif()

## Boost
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
_find_package(Boost REQUIRED COMPONENTS system filesystem program_options iostreams regex)

## In the case the cnr_yamlDependencies is imported from CMakeLists.txt of the 
## cnr_yaml project, the following variables are defined
## Otherwise, they are defined in the cnr_yamlConfig.cmake generated in the
## install configuration steps
if(${PROJECT_NAME} STREQUAL "cnr_yaml")

  # Eigen
  list(APPEND DEPENDENCIES_TARGETS Eigen3::Eigen)
  list(APPEND DEPENDENCIES_INCLUDE_DIRS  ${EIGEN3_INCLUDE_DIRS})
  #list(APPEND DEPENDENCIES_LINK_LIBRARIES "")
  
  # yaml-cpp
  list(APPEND DEPENDENCIES_TARGETS yaml-cpp::yaml-cpp)
  # list(APPEND DEPENDENCIES_INCLUDE_DIRS  /usr/local)

  # Boost
  list(APPEND DEPENDENCIES_TARGETS Boost::system Boost::filesystem Boost::program_options Boost::iostreams Boost::regex)
  # list(APPEND DEPENDENCIES_INCLUDE_DIRS  /usr/local)
  list(APPEND DEPENDENCIES_LINK_LIBRARIES ${Boost_FILESYSTEM_LIBRARY_RELEASE} 
            ${Boost_SYSTEM_LIBRARY_RELEASE} ${Boost_PROGRAM_OPTIONS_LIBRARY_RELEASE} ${Boost_IOSTREAMS_LIBRARY_RELEASE} 
            ${Boost_REGEX_LIBRARY_RELEASE})

  ## Catkin is a dependency-fake package: if I found it it does mean that I sourced ROS into the shell I am building
  ## => therefore, I assume that I want to follow the 'catkin'-way in installing the packages
  _find_package(catkin)
  message(STATUS "CATKIN ENVIRONMENT DETECTED=${catkin_FOUND}")
endif()

