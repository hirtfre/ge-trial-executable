#include "Spi.h"
#include <AbstractionLayer/IMemory.h>
#include <AbstractionLayer/ISpiFactory.h>
#include <GeLib/Endianness.h>
#include <algorithm>
#include <array>
#include <climits>
#include <cstdint>
#include <cstring>
#include <new>

namespace Spi {

static std::array<Spi *, spiConfig.size()> spiList{};

ISpi *SpiFactory::Create(Type type)
{
    Spi *spi = nullptr;
    auto index = static_cast<uint8_t>(type);

    spi = spiList[index];

    if (spi != nullptr)
    {
        return spi;
    }

    auto memory = MEM_Calloc(1, sizeof(Spi));

    if (memory == nullptr)
    {
        goto unable_to_allocate_memory;
    }

    spi = new (memory) Spi();

    if (!spi->Init(spiConfig[index]))
    {
        goto unable_to_initialize_spi;
    }

    spiList[index] = spi;

    return spi;

unable_to_initialize_spi:
    MEM_Free(memory);

unable_to_allocate_memory:
    return nullptr;
}

void SpiFactory::Destroy(ISpi **instance)
{
    if (instance == nullptr || *instance == nullptr)
    {
        return;
    }

    auto *spi = static_cast<Spi *>(*instance);

    std::ranges::replace(spiList, spi, nullptr);

    spi->~Spi();
    MEM_Free(*instance);

    *instance = nullptr;
    instance = nullptr;
}

Spi::~Spi()
{
    DeInit();
}

bool Spi::Init(const SpiConfig &config)
{
    hostDevice = config.host_device;

    spi_bus_config_t busConfig;
    std::memcpy(&busConfig, &config.busConfig, sizeof(spi_bus_config_t));

    if (spi_bus_initialize(hostDevice, &busConfig, SPI_DMA_CH_AUTO) != ESP_OK)
    {
        goto unable_to_initialize_spi_bus;
    }

    spi_device_interface_config_t deviceInterfaceConfig;
    memcpy(&deviceInterfaceConfig, &config.deviceInterfaceConfig,
           sizeof(spi_device_interface_config_t));

    if (spi_bus_add_device(hostDevice, &deviceInterfaceConfig, &spiDeviceHandle) != ESP_OK)
    {
        goto unable_to_add_device;
    }

    return true;

    spi_bus_remove_device(spiDeviceHandle);

unable_to_add_device:
    spi_bus_free(hostDevice);

unable_to_initialize_spi_bus:
    return false;
}

void Spi::DeInit()
{
    spi_bus_remove_device(spiDeviceHandle);
    spi_bus_free(hostDevice);
}

bool Spi::ReadBlocking(DataFrame &dataFrame)
{
    return PollingTransmit(dataFrame, true);
}

bool Spi::WriteBlocking(const DataFrame &dataFrame)
{
    return PollingTransmit(dataFrame, false);
}

inline bool Spi::PollingTransmit(const DataFrame &dataFrame, bool read)
{
    spi_transaction_t transaction = {};
    uint8_t txBuffer[dataFrame.rxLength] = {0};

    transaction.length = dataFrame.txLength * CHAR_BIT;
    transaction.tx_buffer = dataFrame.txBuffer;

    if (read)
    {
        if (dataFrame.rxLength > dataFrame.txLength)
        {
            memcpy(txBuffer, dataFrame.txBuffer, dataFrame.txLength);
            transaction.length = dataFrame.rxLength * CHAR_BIT;
            transaction.tx_buffer = txBuffer;
        }

        transaction.rxlength = dataFrame.rxLength * CHAR_BIT;
        transaction.rx_buffer = dataFrame.rxBuffer;
    }

    esp_err_t ret = spi_device_polling_transmit(spiDeviceHandle, &transaction);

    return ret == ESP_OK;
}

} // namespace Spi