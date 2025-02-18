# https://raw.githubusercontent.com/cmake-modules/lcov/master/coverage.cmake
function(add_coverage_target exclude)

    find_program(GCOV gcov)
    if (NOT GCOV)
        message(WARNING "program gcov not found")
    endif()

    find_program(LCOV lcov)
    if (NOT LCOV)
        message(WARNING "program lcov not found")
    endif()

    find_program(GENHTML genhtml)
    if (NOT GENHTML)
        message(WARNING "program genhtml not found")
    endif()

    if (LCOV AND GCOV AND GENHTML)
        set(covname cov.info)
        add_compile_options(-fprofile-arcs -ftest-coverage)
        add_link_options(--coverage)
        add_custom_target(cov DEPENDS ${covname})
        add_custom_command(
            OUTPUT  ${covname}
            COMMAND gcovr   --html coverage.html 
                            --xml-pretty 
                            --exclude-unreachable-branches 
                            --exclude-throw-branches 
                            --print-summary 
                            -o coverage.xml 
                            --root ${CMAKE_SOURCE_DIR} 
                            -e ${exclude}
            
            # COMMAND ${LCOV} -c -o ${covname} -d . -b . --gcov-tool ${GCOV} --ignore-errors mismatch
            # COMMAND ${LCOV} -r ${covname} ${exclude} -o ${covname} 
            # COMMAND ${LCOV} -l ${covname}
            # COMMAND ${GENHTML} ${covname} -output coverage
            # COMMAND ${LCOV} -l ${covname} 2>/dev/null | grep Total | sed 's/|//g' | sed 's/Total://g' | awk '{print $1}' | sed s/%//g > coverage/total
        )
        set_directory_properties(PROPERTIES
            ADDITIONAL_CLEAN_FILES ${covname}
        )
    else()
        message(WARNING "unable to add target `cov`: missing coverage tools")
    endif()

endfunction()