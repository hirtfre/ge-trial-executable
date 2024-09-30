#ifndef OSAL_ESP_IDF_SPI
#define OSAL_ESP_IDF_SPI

#include "SpiConfig.hpp"
#include "driver/spi_master.h"
#include <AbstractionLayer/ISpi.h>

namespace Spi {

class Spi : public ISpi
{
  public:
    Spi() = default;
    ~Spi() override;

    Spi &operator=(Spi &&) = delete;

    bool Init(const SpiConfig &config);

    bool ReadBlocking(DataFrame &dataFrame) final;
    bool WriteBlocking(const DataFrame &dataFrame) final;

  private:
    spi_device_handle_t spiDeviceHandle;
    spi_host_device_t hostDevice;

    void DeInit();
    inline bool PollingTransmit(const DataFrame &dataFrame, bool read);
};

} // namespace Spi

#endif /* OSAL_ESP_IDF_SPI */
