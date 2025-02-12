# ##############################################################################
# It does takes inspiration from Ton idea is to have a single file that can be
# included in the CMakeLists.txt of the package and in the >package>Config.cmake
# https://discourse.cmake.org/t/how-to-conditionally-call-either-find-pacakge-or-find-dependency/8175
if(${PROJECT_NAME} STREQUAL "cnr_yaml")
  macro(_find_package)
    find_package(${ARGN})
  endmacro()
else()
  include(CMakeFindDependencyMacro)
  macro(_find_package)
    find_dependency(${ARGN})
  endmacro()
endif()
# ##############################################################################

# Eigen
_find_package(Eigen3 REQUIRED COMPONENTS Core Dense Geometry)

# yaml-cpp
_find_package(yaml-cpp REQUIRED)
if(yaml-cpp VERSION_LESS "0.8")
  _find_package(PkgConfig REQUIRED)
  pkg_check_modules(YAML_CPP REQUIRED yaml-cpp IMPORTED_TARGET GLOBAL)
  add_library(yaml-cpp::yaml-cpp ALIAS PkgConfig::YAML_CPP)
endif()

# Boost
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
_find_package(
  Boost
  REQUIRED
  COMPONENTS
  system
  filesystem
  program_options
  iostreams
  regex)

  if(POLICY CMP0167)
  cmake_policy(SET CMP0167 NEW)
endif()