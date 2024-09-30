#ifndef CONFIG_SPI_CONFIG_H
#define CONFIG_SPI_CONFIG_H

#include <array>
#include <driver/spi_master.h>

/********************************
* Auto generated file
* DO NOT EDIT
*********************************/

namespace Spi {

struct SpiConfig
{
    spi_host_device_t host_device;
    spi_bus_config_t busConfig;
    spi_device_interface_config_t deviceInterfaceConfig;
};

static constexpr std::array<SpiConfig, 1> spiConfig = {{
    // ADE7953
    {
        .host_device = SPI2_HOST,
        .busConfig = 
            {
                .mosi_io_num = 5,
                .miso_io_num = 0,
                .sclk_io_num = 4,
                .quadwp_io_num = 1,
                .quadhd_io_num = 1,
                .data4_io_num = -1,
                .data5_io_num = -1,
                .data6_io_num = -1,
                .data7_io_num = -1,
                .max_transfer_sz = 32,
                .flags = 0,
                .isr_cpu_id = ESP_INTR_CPU_AFFINITY_AUTO,
                .intr_flags = 0,
            },
        .deviceInterfaceConfig =
            {
                .command_bits = 0,
                .address_bits = 0,
                .dummy_bits = 0,
                .mode = 0,
                .clock_source = SPI_CLK_SRC_DEFAULT,
                .duty_cycle_pos = 0,
                .cs_ena_pretrans = 0,
                .cs_ena_posttrans = 0,
                .clock_speed_hz = 5000000,
                .input_delay_ns = 0,
                .spics_io_num = 1,
                .flags = 0,
                .queue_size = 1,
                .pre_cb = nullptr,
                .post_cb = nullptr,
            },
    },
}};

enum class Type : uint8_t
{
    ADE7953 = 0,
};

} // namespace Spi

#endif /* CONFIG_SPI_CONFIG_H */
