cmake_minimum_required(VERSION 3.19.8)

function(GENERATE_CLOCK_OUTPUT_CONFIG_HEADER)
    # Read the json file containing the Clock Output configs and create the config array
    file(READ "${CMAKE_CURRENT_LIST_DIR}/config/ClockOutputConfig.json" CLOCK_OUTPUTS_JSON)

    string(JSON CLOCK_OUTPUT_COUNT LENGTH "${CLOCK_OUTPUTS_JSON}" "clockOutputs")
    message(STATUS "Found ${CLOCK_OUTPUT_COUNT} Clock Output configs")

    #CMake uses an inclusive range, so subtract 1 before iterating through all items
    MATH(EXPR INDEX_RANGE "${CLOCK_OUTPUT_COUNT} - 1")

    # Loop through all entries in config list and append to string
    foreach (INDEX RANGE "${INDEX_RANGE}")
        string(JSON CONFIG_NAME GET ${CLOCK_OUTPUTS_JSON} clockOutputs ${INDEX} name)
        string(JSON CONFIG_SPEED_MODE GET ${CLOCK_OUTPUTS_JSON} clockOutputs ${INDEX} speed_mode)
        string(JSON CONFIG_TIMER_NUM GET ${CLOCK_OUTPUTS_JSON} clockOutputs ${INDEX} timer_num)
        string(JSON CONFIG_DUTY_RESOLUTION GET ${CLOCK_OUTPUTS_JSON} clockOutputs ${INDEX} duty_resolution)
        string(JSON CONFIG_FREQ_HZ GET ${CLOCK_OUTPUTS_JSON} clockOutputs ${INDEX} freq_hz)
        string(JSON CONFIG_CLK_CFG GET ${CLOCK_OUTPUTS_JSON} clockOutputs ${INDEX} clk_cfg)
        string(JSON CONFIG_CHANNEL GET ${CLOCK_OUTPUTS_JSON} clockOutputs ${INDEX} channel)
        string(JSON CONFIG_INTR_TYPE GET ${CLOCK_OUTPUTS_JSON} clockOutputs ${INDEX} intr_type)
        string(JSON CONFIG_GPIO_NUM GET ${CLOCK_OUTPUTS_JSON} clockOutputs ${INDEX} gpio_num)
        string(JSON CONFIG_DUTY GET ${CLOCK_OUTPUTS_JSON} clockOutputs ${INDEX} duty)
        string(JSON CONFIG_HPOINT GET ${CLOCK_OUTPUTS_JSON} clockOutputs ${INDEX} hpoint)

        message(STATUS "${CONFIG_NAME}: ${CONFIG_PIN_NUMBER}")
        string(APPEND CLOCK_OUTPUT_CONFIG_ARRAY "\n    // ${CONFIG_NAME}
    {
        .timerConfig = 
            {
                .speed_mode = ${CONFIG_SPEED_MODE},
                .duty_resolution = ${CONFIG_DUTY_RESOLUTION},
                .timer_num = ${CONFIG_TIMER_NUM},
                .freq_hz = ${CONFIG_FREQ_HZ},
                .clk_cfg = ${CONFIG_CLK_CFG},
            },
        .channelConfig =
            {
                .gpio_num = ${CONFIG_GPIO_NUM},
                .speed_mode = ${CONFIG_SPEED_MODE},
                .channel = ${CONFIG_CHANNEL},
                .intr_type = ${CONFIG_INTR_TYPE},
                .timer_sel = ${CONFIG_TIMER_NUM},
                .duty = ${CONFIG_DUTY},
                .hpoint = ${CONFIG_HPOINT},
            },
    },")

        if("${INDEX}" EQUAL "${INDEX_RANGE}")
            string(APPEND CLOCK_OUTPUT_CONFIG_ARRAY "\n")
        endif()

        string(APPEND CLOCK_OUTPUT_CONFIG_TYPES "    ${CONFIG_NAME} = ${INDEX},\n")
    endforeach()

    # Configure the header file containing the configs
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/config/headerTemplates/ClockOutputConfig.h.in" "${CMAKE_CURRENT_SOURCE_DIR}/esp-idf/ClockOutputConfig.hpp" ESCAPE_QUOTES)
endfunction()