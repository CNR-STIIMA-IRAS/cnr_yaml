#
# get_project_name
#
macro(get_project_name filename extracted_name extracted_version)
  # Read the package manifest.
  file(READ "${CMAKE_CURRENT_SOURCE_DIR}/${filename}" package_xml_str)

  # Extract project name.
  if(NOT package_xml_str MATCHES "<name>([A-Za-z0-9_]+)</name>")
    message(
      FATAL_ERROR
        "Could not parse project name from package manifest (aborting)")
  else()
    set(extracted_name ${CMAKE_MATCH_1})
  endif()

  # Extract project version.
  if(NOT package_xml_str MATCHES "<version>([0-9]+.[0-9]+.[0-9]+)</version>")
    message(
      FATAL_ERROR
        "Could not parse project version from package manifest (aborting)")
  else()
    set(extracted_version ${CMAKE_MATCH_1})
  endif()

  if("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" LESS 3.0)
    cmake_policy(SET CMP0048 OLD)
  else()
    cmake_policy(SET CMP0048 NEW)
  endif()

endmacro()

#
# cnr_set_flags
#
macro(cnr_set_flags)
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W3" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  endif()

  if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
  endif()

  if(NOT CMAKE_C_STANDARD)
    set(CMAKE_C_STANDARD 99)
  endif()

  set(LOCAL_CXX_STANDARD 20)
  if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")

    add_compile_options(-Wall -Wextra -Wpedantic -D_TIME_BITS=64
                        -D_FILE_OFFSET_BITS=64)

    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND CMAKE_CXX_COMPILER_VERSION GREATER 12)
      set(LOCAL_CXX_STANDARD 20)
    elseif(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_CXX_COMPILER_VERSION GREATER 10)
      set(LOCAL_CXX_STANDARD 20)
    endif()

  endif()

  # Default to C++20
  if(NOT CMAKE_CXX_STANDARD)
    message(STATUS "CMAKE CXX STANDARD: ${LOCAL_CXX_STANDARD}")
    set(CMAKE_CXX_STANDARD ${LOCAL_CXX_STANDARD})
  endif()

  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)

  if(${CMAKE_VERSION} VERSION_GREATER "3.16.0")
    set(THREADS_PREFER_PTHREAD_FLAG ON)
  endif()
endmacro()

#
# cnr_target_compile_options
#
macro(cnr_target_compile_options TARGET_NAME)

  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")

    target_compile_options(
      ${TARGET_NAME}
      PRIVATE -Wall -Wextra -Wunreachable-code -Wpedantic
      PUBLIC $<$<CONFIG:Release>:-Ofast -funroll-loops -ffast-math >)

  elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")

    target_compile_options(
      ${TARGET_NAME}
      PRIVATE -Wweak-vtables -Wexit-time-destructors -Wglobal-constructors
              -Wmissing-noreturn -Wno-gnu-zero-variadic-macro-arguments
      PUBLIC $<$<CONFIG:Release>:-Ofast -funroll-loops -ffast-math >)

  elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")

    target_compile_options(${TARGET_NAME} PRIVATE /W3)

  endif()
endmacro()

#
# cnr_install
#
macro(
  cnr_install
  LIBRARY_TARGETS_LIST
  EXECUTABLE_TARGETS_LIST
  HEADERS_DIRS)

  list(APPEND TARGETS_LIST ${LIBRARY_TARGETS_LIST} ${EXECUTABLE_TARGETS_LIST})
  
  # We create the congfig files for the package
  set(CONFIG_INSTALL_DIR_ALTERNATIVE "${CONFIG_INSTALL_DIR}/../cmake_alternative")
  set(CONFIG_NAMESPACE               "${PROJECT_NAME}::")
  set(TARGETS_EXPORT_NAME            "${PROJECT_NAME}Targets")
  set(VERSION_CONFIG                 "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake")
  set(PROJECT_CONFIG_OUTPUT          "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake")
  set(PROJECT_CONFIG_INPUT_TEMPLATE  "cmake/cnr_yamlConfig.cmake.in")

  # Parameter template in the PROJECT_CONFIG_INPUT_TEMPLATE
  list(APPEND DEPENDENCIES_INCLUDE_DIRS  ${PACKAGE_INCLUDE_DESTINATION})
  list(REMOVE_DUPLICATES DEPENDENCIES_INCLUDE_DIRS)
  set(EXPORTED_TARGET_INCLUDE_DIRS      "${DEPENDENCIES_INCLUDE_DIRS}")

  list(APPEND DEPENDENCIES_LINK_LIBRARIES  ${LIBRARY_TARGETS_LIST})
  set(EXPORTED_LIBRARY_TARGETS_LIST        "${DEPENDENCIES_LINK_LIBRARIES}")
  set(EXPORTED_EXECUTABLE_TARGETS_LIST     "${EXECUTABLE_TARGETS_LIST}")
  set(EXPORTED_LIBRARY_TARGET_RPATH        "${PACKAGE_LIB_DESTINATION}")
  
  ##
  message(STATUS "CONFIG_INSTALL_DIR              = ${CONFIG_INSTALL_DIR}"           )
  message(STATUS "CONFIG_NAMESPACE                = ${CONFIG_NAMESPACE}"             )
  message(STATUS "TARGETS_EXPORT_NAME             = ${TARGETS_EXPORT_NAME}"          )
  message(STATUS "VERSION_CONFIG                  = ${VERSION_CONFIG}"               )
  message(STATUS "PROJECT_CONFIG_OUTPUT           = ${PROJECT_CONFIG_OUTPUT}"        )
  message(STATUS "PROJECT_CONFIG_INPUT_TEMPLATE   = ${PROJECT_CONFIG_INPUT_TEMPLATE}")
  message(STATUS "EXPORTED_TARGET_INCLUDE_DIRS    = ${EXPORTED_TARGET_INCLUDE_DIRS}"  )
  message(STATUS "EXPORTED_LIBRARY_TARGETS_LIST   = ${EXPORTED_LIBRARY_TARGETS_LIST}" )
  message(STATUS "EXPORTED_EXECUTABLE_TARGETS_LIST= ${EXECUTABLE_TARGETS_LIST}"      )

  # 1 install files
  foreach(HEADERS_DIR ${HEADERS_DIRS})
    install(
      DIRECTORY ${HEADERS_DIR}/
      DESTINATION "${PACKAGE_INCLUDE_DESTINATION}"
      FILES_MATCHING
      PATTERN "*.h"
      PATTERN "*.hpp"
      PATTERN ".svn" EXCLUDE)
  endforeach()

  install(
    TARGETS ${TARGETS_LIST}
    EXPORT ${TARGETS_EXPORT_NAME}
    ARCHIVE DESTINATION "${PACKAGE_LIB_DESTINATION}"
    LIBRARY DESTINATION "${PACKAGE_LIB_DESTINATION}"
    RUNTIME DESTINATION "${PACKAGE_BIN_DESTINATION}")

  include(CMakePackageConfigHelpers)

  #------------------------------------------------------------------------------
  # Configure <PROJECT_NAME>ConfigVersion.cmake common to build and install tree
  write_basic_package_version_file(
    "${VERSION_CONFIG}"
    VERSION ${extracted_version}
    COMPATIBILITY SameMajorVersion)

  #------------------------------------------------------------------------------
  # Create the ${PROJECT_NAME}Config.cmake using the template
  # ${PROJECT_NAME}Config.cmake.in
  configure_package_config_file(
    "${PROJECT_CONFIG_INPUT_TEMPLATE}" "${PROJECT_CONFIG_OUTPUT}"
    INSTALL_DESTINATION "${CONFIG_INSTALL_DIR}")

  #------------------------------------------------------------------------------
  # Install cmake targets files
  install(
    EXPORT "${TARGETS_EXPORT_NAME}"
    NAMESPACE "${CONFIG_NAMESPACE}"
    DESTINATION "${CONFIG_INSTALL_DIR}"
    FILE ${TARGETS_EXPORT_NAME}.cmake
  )
  
  #------------------------------------------------------------------------------
  # Install cmake config files
  install(FILES "${PROJECT_CONFIG_OUTPUT}" "${VERSION_CONFIG}" "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cnr_yamlDependencies.cmake"
          DESTINATION "${CONFIG_INSTALL_DIR}")

  install(FILES "${PROJECT_CONFIG_OUTPUT}" "${VERSION_CONFIG}" "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cnr_yamlDependencies.cmake"
          DESTINATION "${CONFIG_INSTALL_DIR_ALTERNATIVE}")
endmacro()

#
# cnr_configure_gtest
#
macro(cnr_configure_gtest trg deps)

  find_package(GTest REQUIRED)

  if(${GTest_FOUND})
    include(GoogleTest)
    enable_testing()
    include(CTest REQUIRED)

    gtest_discover_tests(${trg})

    if(${CMAKE_VERSION} VERSION_GREATER "3.16.0")
      target_link_libraries(
        ${trg}
        PUBLIC
        ${deps}
        Threads::Threads
        GTest::Main
        Boost::program_options
        Boost::system
        Boost::filesystem
        Boost::iostreams
        Boost::regex)
    else()
      target_link_libraries(${trg}
        ${dep} GTest::Main)
      if(THREADS_HAVE_PTHREAD_ARG)
        target_compile_options(${trg} PUBLIC "-pthread")
      endif()
      if(CMAKE_THREAD_LIBS_INIT)
        target_link_libraries(${trg}
                              "${CMAKE_THREAD_LIBS_INIT}")
      endif()
    endif()
  else()

    message(WARNING "unable to add gtest: missing pacakge GTest")

  endif()
endmacro()


# Get all propreties that cmake supports
if(NOT CMAKE_PROPERTY_LIST)
    execute_process(COMMAND cmake --help-property-list OUTPUT_VARIABLE CMAKE_PROPERTY_LIST)
    
    # Convert command output into a CMake list
    string(REGEX REPLACE ";" "\\\\;" CMAKE_PROPERTY_LIST "${CMAKE_PROPERTY_LIST}")
    string(REGEX REPLACE "\n" ";" CMAKE_PROPERTY_LIST "${CMAKE_PROPERTY_LIST}")
    list(REMOVE_DUPLICATES CMAKE_PROPERTY_LIST)
endif()
    
function(print_properties)
    message("CMAKE_PROPERTY_LIST = ${CMAKE_PROPERTY_LIST}")
endfunction()
    
function(print_target_properties target)
    if(NOT TARGET ${target})
      message(STATUS "There is no target named '${target}'")
      return()
    endif()

    foreach(property ${CMAKE_PROPERTY_LIST})
        string(REPLACE "<CONFIG>" "${CMAKE_BUILD_TYPE}" property ${property})

        # Fix https://stackoverflow.com/questions/32197663/how-can-i-remove-the-the-location-property-may-not-be-read-from-target-error-i
        if(property STREQUAL "LOCATION" OR property MATCHES "^LOCATION_" OR property MATCHES "_LOCATION$")
            continue()
        endif()

        get_property(was_set TARGET ${target} PROPERTY ${property} SET)
        if(was_set)
            get_target_property(value ${target} ${property})
            message("${target} ${property} = ${value}")
        endif()
    endforeach()
endfunction()

