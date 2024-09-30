cmake_minimum_required(VERSION 3.19.8)

function(GENERATE_PERIPHERAL_TYPES_HEADER PERIPHERAL_CONFIG_JSON_PATH PERIPHERAL_TYPES_HEADER_TEMPLATE PERIPHERAL_TYPES_HEADER_FILENAME PERIPHERAL_KEY)
    # Read the JSON configuration file
    file(READ "${PERIPHERAL_CONFIG_JSON_PATH}" PERIPHERAL_CONFIG_JSON)

    # Get the total number of peripherals
    string(JSON PERIPHERAL_COUNT LENGTH "${PERIPHERAL_CONFIG_JSON}" "${PERIPHERAL_KEY}")
    MATH(EXPR PERIPHERAL_RANGE "${PERIPHERAL_COUNT} - 1")

    set(MODULE_NAMES_SET "")

    # Collect all unique module names
    foreach (INDEX RANGE ${PERIPHERAL_RANGE})
        string(JSON MODULE_COUNT LENGTH "${PERIPHERAL_CONFIG_JSON}" "${PERIPHERAL_KEY}" ${INDEX} modules)
        MATH(EXPR MODULE_RANGE "${MODULE_COUNT} - 1")

        foreach (MODULE_INDEX RANGE ${MODULE_RANGE})
            string(JSON PERIPHERAL_MODULE_NAME GET ${PERIPHERAL_CONFIG_JSON} "${PERIPHERAL_KEY}" ${INDEX} modules ${MODULE_INDEX})

            list(FIND MODULE_NAMES_SET "${PERIPHERAL_MODULE_NAME}" MODULE_EXISTS)
            if(MODULE_EXISTS EQUAL -1)
                list(APPEND MODULE_NAMES_SET "${PERIPHERAL_MODULE_NAME}")
            endif()
        endforeach()
    endforeach()

    # Generate a header file for each module
    foreach(MODULE_NAME IN LISTS MODULE_NAMES_SET)
        set(PERIPHERAL_TYPES_ENUM "")

        foreach (INDEX RANGE ${PERIPHERAL_RANGE})
            # Extract Peripheral name and module information
            string(JSON PERIPHERAL_NAME GET ${PERIPHERAL_CONFIG_JSON} "${PERIPHERAL_KEY}" ${INDEX} name)
            string(JSON MODULE_COUNT LENGTH "${PERIPHERAL_CONFIG_JSON}" "${PERIPHERAL_KEY}" ${INDEX} modules)
            MATH(EXPR MODULE_RANGE "${MODULE_COUNT} - 1")

            foreach (MODULE_INDEX RANGE ${MODULE_RANGE})
                string(JSON PERIPHERAL_MODULE_NAME GET ${PERIPHERAL_CONFIG_JSON} "${PERIPHERAL_KEY}" ${INDEX} modules ${MODULE_INDEX})

                # If the module name matches, append the peripheral name to the enum string
                if("${PERIPHERAL_MODULE_NAME}" STREQUAL "${MODULE_NAME}")
                    string(APPEND PERIPHERAL_TYPES_ENUM "\n    ${PERIPHERAL_NAME} = ${INDEX},")
                    break()
                endif()
            endforeach()
        endforeach()

        message(STATUS "${MODULE_NAME} found ${PERIPHERAL_KEY}: ${PERIPHERAL_TYPES_ENUM}")

        # Generate the output header file using the collected enum entries
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${PERIPHERAL_TYPES_HEADER_TEMPLATE}
                       ${CMAKE_SOURCE_DIR}/apps/${MODULE_NAME}/src/${PERIPHERAL_TYPES_HEADER_FILENAME}
                       @ONLY)
    endforeach()
endfunction()
