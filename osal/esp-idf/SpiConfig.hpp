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

static constexpr std::array<SpiConfig, 0> spiConfig;

enum class Type : uint8_t
{
    None,
};

} // namespace Spi

#endif /* CONFIG_SPI_CONFIG_H */
