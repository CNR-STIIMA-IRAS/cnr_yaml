#
# cnr_install
#
macro(
  cnr_install
  LIBRARY_TARGETS_LIST
  EXECUTABLE_TARGETS_LIST
  HEADERS_DIRS)

  if(BUILD_AS_A_CATKIN_PACKAGE)
    ########################
    # Catkin Package
    catkin_package(
      INCLUDE_DIRS "${HEADERS_DIRS}"
      LIBRARIES "${LIBRARY_TARGETS_LIST}"
      DEPENDS "${DEPENDENCIES_KEY}"
      CFG_EXTRAS "${CMAKE_CURRENT_LIST_DIR}/cmake/cnrCfgExtras.cmake"
      EXPORTED_TARGETS "${LIBRARIES_TARGETS_LIST}"
    )
    ########################
  else()

    set(PACKAGE_LIB_DESTINATION        "lib")
    set(PACKAGE_BIN_DESTINATION        "bin")
    set(PACKAGE_INCLUDE_DESTINATION    "include")
    set(PACKAGE_CONFIG_DESTINATION     "share/${PROJECT_NAME}/cmake")
    set(CONFIG_NAMESPACE               "${PROJECT_NAME}::")
    set(TARGETS_EXPORT_NAME            "${PROJECT_NAME}Targets")
    set(VERSION_CONFIG                 "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake")
    set(PROJECT_CONFIG_OUTPUT          "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake")
    set(PROJECT_CONFIG_INPUT_TEMPLATE  "cmake/cnrConfig.cmake.in")

    # Parameter template in the PROJECT_CONFIG_INPUT_TEMPLATE
    list(APPEND DEPENDENCIES_INCLUDE_DIRS "${CMAKE_INSTALL_PREFIX}/${PACKAGE_INCLUDE_DESTINATION}")
    list(REMOVE_DUPLICATES DEPENDENCIES_INCLUDE_DIRS)

    # Merge targets
    foreach(_LIBRARY_TARGET ${LIBRARY_TARGETS_LIST})
      get_target_property(target_type ${_LIBRARY_TARGET} TYPE)
      if (target_type STREQUAL "SHARED_LIBRARY")
      get_target_property(target_name ${_LIBRARY_TARGET} OUTPUT_NAME)
        list(APPEND DEPENDENCIES_LINK_LIBRARIES "${CMAKE_INSTALL_PREFIX}/${PACKAGE_LIB_DESTINATION}/${CMAKE_SHARED_LIBRARY_PREFIX}${target_name}${CMAKE_SHARED_LIBRARY_SUFFIX}")
      elseif (target_type STREQUAL "STATIC_LIBRARY")
        get_target_property(target_name ${_LIBRARY_TARGET} OUTPUT_NAME)
        list(APPEND DEPENDENCIES_LINK_LIBRARIES "${CMAKE_INSTALL_PREFIX}/${PACKAGE_LIB_DESTINATION}/${CMAKE_STATIC_LIBRARY_PREFIX}${target_name}${CMAKE_STATIC_LIBRARY_SUFFIX}")
      endif()
    endforeach()
    list(REMOVE_DUPLICATES DEPENDENCIES_LINK_LIBRARIES)
    
    foreach( _EXE_TARGET ${EXECUTABLE_TARGETS_LIST})
      get_target_property(target_name ${_EXE_TARGET} OUTPUT_NAME)
      list(APPEND DEPENDENCIES_EXE "${CMAKE_INSTALL_PREFIX}/${PACKAGE_LIB_DESTINATION}/${target_name}")
    endforeach()
    list(REMOVE_DUPLICATES DEPENDENCIES_INCLUDE_DIRS)
    
    set(EXPORTED_TARGET_INCLUDE_DIRS      "${DEPENDENCIES_INCLUDE_DIRS}")
    set(EXPORTED_LIBRARY_TARGETS_LIST     "${DEPENDENCIES_LINK_LIBRARIES}")
    set(EXPORTED_EXECUTABLE_TARGETS_LIST  "${DEPENDENCIES_EXE}")
    set(EXPORTED_LIBRARY_TARGET_RPATH     "${CMAKE_INSTALL_PREFIX}/${PACKAGE_LIB_DESTINATION}")

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

    list(APPEND TARGETS_LIST ${LIBRARY_TARGETS_LIST} ${EXECUTABLE_TARGETS_LIST})
    install(
      TARGETS ${TARGETS_LIST}
      EXPORT ${TARGETS_EXPORT_NAME}
      ARCHIVE DESTINATION "${PACKAGE_LIB_DESTINATION}"
      LIBRARY DESTINATION "${PACKAGE_LIB_DESTINATION}"
      RUNTIME DESTINATION "${PACKAGE_BIN_DESTINATION}")

    include(CMakePackageConfigHelpers)

    file(READ "${CMAKE_CURRENT_LIST_DIR}/cmake/cnrDependencies.cmake" DEPENDENCIES_FILE_CONTENT)
    file(READ "${CMAKE_CURRENT_LIST_DIR}/cmake/cnrCfgExtras.cmake"    CFG_EXTRAS_FILE_CONTENT)
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
      INSTALL_DESTINATION "${PACKAGE_CONFIG_DESTINATION}")

    #------------------------------------------------------------------------------
    # Install cmake targets files and also namespaced alias targets
    add_library(cnr_yaml::cnr_yaml ALIAS cnr_yaml)
    install(
      EXPORT "${TARGETS_EXPORT_NAME}"
      NAMESPACE "${CONFIG_NAMESPACE}"
      DESTINATION "${PACKAGE_CONFIG_DESTINATION}"
      FILE ${TARGETS_EXPORT_NAME}.cmake
    )
    
    #------------------------------------------------------------------------------
    # Install cmake config files
    install(FILES "${PROJECT_CONFIG_OUTPUT}" "${VERSION_CONFIG}" "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cnrDependencies.cmake"
            DESTINATION "${PACKAGE_CONFIG_DESTINATION}")
  endif()

endmacro()
