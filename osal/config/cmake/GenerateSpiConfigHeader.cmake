cmake_minimum_required(VERSION 3.19.8)

function(GENERATE_SPI_CONFIG_HEADER)
    # Read the json file containing the SPI configs and create the config array
    file(READ "${CMAKE_CURRENT_LIST_DIR}/config/SpiConfig.json" SPIS_JSON)

    string(JSON SPI_COUNT LENGTH "${SPIS_JSON}" "spis")
    message(STATUS "Found ${SPI_COUNT} SPI configs")

    #CMake uses an inclusive range, so subtract 1 before iterating through all items
    MATH(EXPR INDEX_RANGE "${SPI_COUNT} - 1")

    # Loop through all entries in config list and append to string
    foreach (INDEX RANGE "${INDEX_RANGE}")
        string(JSON CONFIG_NAME GET ${SPIS_JSON} spis ${INDEX} name)
        string(JSON CONFIG_HOST_DEVICE GET ${SPIS_JSON} spis ${INDEX} host_device)
        string(JSON CONFIG_MISO_IO_NUM GET ${SPIS_JSON} spis ${INDEX} miso_io_num)
        string(JSON CONFIG_MOSI_IO_NUM GET ${SPIS_JSON} spis ${INDEX} mosi_io_num)
        string(JSON CONFIG_SCLK_IO_NUM GET ${SPIS_JSON} spis ${INDEX} sclk_io_num)
        string(JSON CONFIG_QUADWP_IO_NUM GET ${SPIS_JSON} spis ${INDEX} quadwp_io_num)
        string(JSON CONFIG_QUADHD_FILTER GET ${SPIS_JSON} spis ${INDEX} quadhd_io_num)
        string(JSON CONFIG_MAX_TRANSFER_SZ GET ${SPIS_JSON} spis ${INDEX} max_transfer_sz)
        string(JSON CONFIG_CLOCK_SPEED_HZ GET ${SPIS_JSON} spis ${INDEX} clock_speed_hz)
        string(JSON CONFIG_MODE GET ${SPIS_JSON} spis ${INDEX} mode)
        string(JSON CONFIG_FLAGS GET ${SPIS_JSON} spis ${INDEX} flags)
        string(JSON CONFIG_SPICS_IO_NUM GET ${SPIS_JSON} spis ${INDEX} spics_io_num)
        string(JSON CONFIG_COMMAND_BITS GET ${SPIS_JSON} spis ${INDEX} command_bits)
        string(JSON CONFIG_ADDRESS_BITS GET ${SPIS_JSON} spis ${INDEX} address_bits)
        string(JSON CONFIG_DUMMY_BITS GET ${SPIS_JSON} spis ${INDEX} dummy_bits)
        string(JSON CONFIG_QUEUE_SIZE GET ${SPIS_JSON} spis ${INDEX} queue_size)


        message(STATUS "${CONFIG_NAME}: ${CONFIG_PIN_NUMBER}")
        string(APPEND SPI_CONFIG_ARRAY "\n    // ${CONFIG_NAME}
    {
        .host_device = ${CONFIG_HOST_DEVICE},
        .busConfig = 
            {
                .mosi_io_num = ${CONFIG_MOSI_IO_NUM},
                .miso_io_num = ${CONFIG_MISO_IO_NUM},
                .sclk_io_num = ${CONFIG_SCLK_IO_NUM},
                .quadwp_io_num = ${CONFIG_QUADWP_IO_NUM},
                .quadhd_io_num = ${CONFIG_QUADHD_FILTER},
                .data4_io_num = -1,
                .data5_io_num = -1,
                .data6_io_num = -1,
                .data7_io_num = -1,
                .max_transfer_sz = ${CONFIG_MAX_TRANSFER_SZ},
                .flags = 0,
                .isr_cpu_id = ESP_INTR_CPU_AFFINITY_AUTO,
                .intr_flags = 0,
            },
        .deviceInterfaceConfig =
            {
                .command_bits = ${CONFIG_COMMAND_BITS},
                .address_bits = ${CONFIG_ADDRESS_BITS},
                .dummy_bits = ${CONFIG_DUMMY_BITS},
                .mode = ${CONFIG_MODE},
                .clock_source = SPI_CLK_SRC_DEFAULT,
                .duty_cycle_pos = 0,
                .cs_ena_pretrans = 0,
                .cs_ena_posttrans = 0,
                .clock_speed_hz = ${CONFIG_CLOCK_SPEED_HZ},
                .input_delay_ns = 0,
                .spics_io_num = ${CONFIG_SPICS_IO_NUM},
                .flags = ${CONFIG_FLAGS},
                .queue_size = ${CONFIG_QUEUE_SIZE},
                .pre_cb = nullptr,
                .post_cb = nullptr,
            },
    },")

        if("${INDEX}" EQUAL "${INDEX_RANGE}")
            string(APPEND SPI_CONFIG_ARRAY "\n")
        endif()

        string(APPEND SPI_CONFIG_TYPES "    ${CONFIG_NAME} = ${INDEX},\n")
    endforeach()

    # Configure the header file containing the configs
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/config/headerTemplates/SpiConfig.h.in" "${CMAKE_CURRENT_SOURCE_DIR}/esp-idf/SpiConfig.hpp" ESCAPE_QUOTES)
endfunction()