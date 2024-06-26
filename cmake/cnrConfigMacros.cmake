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

  # use, i.e. don't skip the full RPATH for the build tree
  set(CMAKE_SKIP_BUILD_RPATH FALSE)

  # when building, don't use the install RPATH already
  # (but later on when installing)
  set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
  set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
 
  # add the automatically determined parts of the RPATH
  # which point to directories outside the build tree to the install RPATH
  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
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

#
# cnr_install
#
macro(
  cnr_install
  PROJECT_VERSION
  LIBRARY_TARGETS_LIST
  EXECUTABLE_TARGETS_LIST
  HEADERS_DIRS)

  list(APPEND TARGETS_LIST ${LIBRARY_TARGETS_LIST} ${EXECUTABLE_TARGETS_LIST})
  
  ###################################################################
  set(CONFIG_NAMESPACE                "${PROJECT_NAME}::")
  set(TARGETS_EXPORT_NAME             "${PROJECT_NAME}Targets")
  set(VERSION_CONFIG                  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake")
  set(PROJECT_CONFIG_OUTPUT           "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake")
  set(PROJECT_CONFIG_INPUT_TEMPLATE   "cmake/cnrConfigTemplate.cmake.in")

  # Parameter template in the PROJECT_CONFIG_INPUT_TEMPLATE
  set(PACKAGE_INCLUDE_INSTALL_DIR       "${CMAKE_INSTALL_PREFIX}/include")
  set(EXPORTED_LIBRARY_TARGETS_LIST     "${LIBRARY_TARGETS_LIST}")
  set(EXPORTED_EXECUTABLE_TARGETS_LIST  "${EXECUTABLE_TARGETS_LIST}")
  set(EXPORTED_LIBRARY_TARGET_RPATH     "${PACKAGE_LIB_DESTINATION}")
  
  ##
  message(STATUS "CONFIG_INSTALL_DIR=${CONFIG_INSTALL_DIR}")
  message(STATUS "CONFIG_NAMESPACE=${CONFIG_NAMESPACE}")
  message(STATUS "TARGETS_EXPORT_NAME=${TARGETS_EXPORT_NAME}")
  message(STATUS "VERSION_CONFIG=${VERSION_CONFIG}")
  message(STATUS "PROJECT_CONFIG_OUTPUT=${PROJECT_CONFIG_OUTPUT}")
  message(STATUS "PROJECT_CONFIG_INPUT_TEMPLATE=${PROJECT_CONFIG_INPUT_TEMPLATE}")
  message(STATUS "PACKAGE_INCLUDE_INSTALL_DIR=${PACKAGE_INCLUDE_INSTALL_DIR}")
  message(STATUS "LIBRARY_TARGETS_LIST=${LIBRARY_TARGETS_LIST}")
  message(STATUS "EXPORTED_EXECUTABLE_TARGETS_LIST=${EXECUTABLE_TARGETS_LIST}")
  message(STATUS "LIBRARY_TARGETS_LIST=${LIBRARY_TARGETS_LIST}")
  message(STATUS "TARGETS_LIST=${TARGETS_LIST}")
  
  include(CMakePackageConfigHelpers)

  # Create the ${PROJECT_NAME}ConfigVersion.cmake
  write_basic_package_version_file(
    "${VERSION_CONFIG}"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)

  # Create the ${PROJECT_NAME}Config.cmake using the template
  # ${PROJECT_NAME}Config.cmake.in
  configure_package_config_file(
    "${PROJECT_CONFIG_INPUT_TEMPLATE}" "${PROJECT_CONFIG_OUTPUT}"
    INSTALL_DESTINATION "${CONFIG_INSTALL_DIR}")

  # Install cmake config files
  install(FILES "${PROJECT_CONFIG_OUTPUT}" "${VERSION_CONFIG}"
          DESTINATION "${CONFIG_INSTALL_DIR}")
          
  install(
    TARGETS ${TARGETS_LIST}
    EXPORT ${TARGETS_EXPORT_NAME}
    ARCHIVE DESTINATION "${PACKAGE_LIB_DESTINATION}"
    LIBRARY DESTINATION "${PACKAGE_LIB_DESTINATION}"
    RUNTIME DESTINATION "${PACKAGE_BIN_DESTINATION}")

  foreach(HEADERS_DIR ${HEADERS_DIRS})
    install(
      DIRECTORY ${HEADERS_DIR}/
      DESTINATION "${PACKAGE_INCLUDE_DESTINATION}"
      FILES_MATCHING
      PATTERN "*.h"
      PATTERN "*.hpp"
      PATTERN ".svn" EXCLUDE)
  endforeach()

  # Install cmake targets files
  install(
    EXPORT ${TARGETS_EXPORT_NAME}
    NAMESPACE "${CONFIG_NAMESPACE}"
    DESTINATION "${CONFIG_INSTALL_DIR}"
    FILE ${PROJECT_NAME}Targets.cmake
  )
  ###################################################################
endmacro()

