cmake_minimum_required(VERSION 3.19.8)

function(GENERATE_TIMER_CONFIG_HEADER)
    # Read the json file containing the TIMER configs and create the config array
    file(READ "${CMAKE_CURRENT_LIST_DIR}/config/TimerConfig.json" TIMERS_JSON)

    string(JSON TIMER_COUNT LENGTH "${TIMERS_JSON}" "timers")
    message(STATUS "Found ${TIMER_COUNT} TIMER configs")

    #CMake uses an inclusive range, so subtract 1 before iterating through all items
    MATH(EXPR INDEX_RANGE "${TIMER_COUNT} - 1")

    # Loop through all entries in config list and append to string
    foreach (INDEX RANGE "${INDEX_RANGE}")
        string(JSON CONFIG_NAME GET ${TIMERS_JSON} timers ${INDEX} name)
        string(JSON CONFIG_CLOCK_SOURCE GET ${TIMERS_JSON} timers ${INDEX} clockSource)
        string(JSON CONFIG_DIRECTION GET ${TIMERS_JSON} timers ${INDEX} direction)
        string(JSON CONFIG_RESOLUTION GET ${TIMERS_JSON} timers ${INDEX} resolutionHz)
        string(JSON CONFIG_PRIORITY GET ${TIMERS_JSON} timers ${INDEX} interruptPriority)
        string(JSON CONFIG_FLAGS GET ${TIMERS_JSON} timers ${INDEX} flags)

        message(STATUS "${CONFIG_NAME}: ${CONFIG_PIN_NUMBER}")
        string(APPEND TIMER_CONFIG_ARRAY "\n    // ${CONFIG_NAME}
    {
        .config    
            {
                .clk_src = ${CONFIG_CLOCK_SOURCE},
                .direction = ${CONFIG_DIRECTION},
                .resolution_hz = ${CONFIG_RESOLUTION},
                .intr_priority = ${CONFIG_PRIORITY},
                .flags = ${CONFIG_FLAGS},
            }
    },")

        if("${INDEX}" EQUAL "${INDEX_RANGE}")
            string(APPEND TIMER_CONFIG_ARRAY "\n")
        endif()

        string(APPEND TIMER_CONFIG_TYPES "    ${CONFIG_NAME} = ${INDEX},\n")
    endforeach()

    # Configure the header file containing the configs
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/config/headerTemplates/TimerConfig.h.in" "${CMAKE_CURRENT_SOURCE_DIR}/esp-idf/TimerConfig.hpp" ESCAPE_QUOTES)
endfunction()