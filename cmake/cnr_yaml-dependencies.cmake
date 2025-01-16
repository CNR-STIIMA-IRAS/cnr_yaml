# ##############################################################################
# It does takes inspiration from Ton idea is to have a single file that can be
# included in the CMakeLists.txt of the package and in the >package>Config.cmake
# https://discourse.cmake.org/t/how-to-conditionally-call-either-find-pacakge-or-find-dependency/8175
if(${PROJECT_NAME} STREQUAL "cnr_yaml")
  message(
    STATUS
      "Loading cnr_yamlDependencies.cmake for the project '${PROJECT_NAME}'")
  macro(_find_package)
    find_package(${ARGN})
  endmacro()
else()
  message(
    STATUS
      "Running Project: ${PROJECT_NAME}.\
       Loading cnr_yamlDependencies.cmake from cnr_yamlConfig.cmake"
  )
  include(CMakeFindDependencyMacro)

  macro(_find_package)
    find_dependency(${ARGN})
  endmacro()
endif()
# ##############################################################################

message(
  STATUS
    "The 'find_package' will look for modules under the following paths:\
     ${CMAKE_PREFIX_PATH};${CMAKE_FRAMEWORK_PATH};${CMAKE_APPBUNDLE_PATH};\
     ${CMAKE_MODULE_PATH};${CMAKE_SYSTEM_PREFIX_PATH};\
     ${CMAKE_SYSTEM_FRAMEWORK_PATH};${CMAKE_SYSTEM_APPBUNDLE_PATH}"
)

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
