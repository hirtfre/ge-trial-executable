cmake_minimum_required(VERSION 3.19.8)

function(GENERATE_GPIO_CONFIG_HEADER)
    # Read the json file containing the GPIO configs and create the config array
    file(READ "${CMAKE_CURRENT_LIST_DIR}/config/GpioConfig.json" GPIOS_JSON)

    string(JSON GPIO_COUNT LENGTH "${GPIOS_JSON}" "gpios")
    message(STATUS "Found ${GPIO_COUNT} GPIO configs")

    #CMake uses an inclusive range, so subtract 1 before iterating through all items
    MATH(EXPR INDEX_RANGE "${GPIO_COUNT} - 1")

    # Loop through all entries in config list and append to string
    foreach (INDEX RANGE "${INDEX_RANGE}")
        string(JSON CONFIG_NAME GET ${GPIOS_JSON} gpios ${INDEX} name)
        string(JSON CONFIG_MODE GET ${GPIOS_JSON} gpios ${INDEX} mode)
        string(JSON CONFIG_ENABLE_PULL_UP GET ${GPIOS_JSON} gpios ${INDEX} enablePullUp)
        string(JSON CONFIG_ENABLE_PULL_DOWN GET ${GPIOS_JSON} gpios ${INDEX} enablePullDown)
        string(JSON CONFIG_INTERRUPT_TYPE GET ${GPIOS_JSON} gpios ${INDEX} interruptType)
        string(JSON CONFIG_HYSTEREIS_FILTER GET ${GPIOS_JSON} gpios ${INDEX} hysteresisFilter)
        string(JSON CONFIG_PIN_NUMBER GET ${GPIOS_JSON} gpios ${INDEX} pinNumber)
        string(JSON CONFIG_IS_ACTIVE_LOW GET ${GPIOS_JSON} gpios ${INDEX} isActiveLow)

        message(STATUS "${CONFIG_NAME}: ${CONFIG_PIN_NUMBER}")
        string(APPEND GPIO_CONFIG_ARRAY "\n    // ${CONFIG_NAME}
    {
        .config = 
            {
                .mode = ${CONFIG_MODE},
                .pull_up_en = ${CONFIG_ENABLE_PULL_UP},
                .pull_down_en = ${CONFIG_ENABLE_PULL_DOWN},
                .intr_type = ${CONFIG_INTERRUPT_TYPE},
                .hys_ctrl_mode = ${CONFIG_HYSTEREIS_FILTER},
            },
        .pinNumber = ${CONFIG_PIN_NUMBER},
        .isActiveLow = ${CONFIG_IS_ACTIVE_LOW},
    },")

        if("${INDEX}" EQUAL "${INDEX_RANGE}")
            string(APPEND GPIO_CONFIG_ARRAY "\n")
        endif()

        string(APPEND GPIO_CONFIG_TYPES "    ${CONFIG_NAME} = ${INDEX},\n")
    endforeach()

    # Configure the header file containing the configs
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/config/headerTemplates/GpioConfig.h.in" "${CMAKE_CURRENT_SOURCE_DIR}/esp-idf/GpioConfig.hpp" ESCAPE_QUOTES)
endfunction()