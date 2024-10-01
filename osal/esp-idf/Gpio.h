#ifndef OSAL_ESP_IDF_GPIO
#define OSAL_ESP_IDF_GPIO

#include "GpioConfig.hpp"
#include <AbstractionLayer/IGpio.h>
#include <driver/gpio.h>

namespace Gpio {

class Gpio : public IGpio
{
  public:
    Gpio() = default;
    ~Gpio() override = default;

    Gpio &operator=(Gpio &&) = delete;

    bool Read() override;
    bool Write(bool value) override;
    bool On() override;
    bool Off() override;
    bool EnableInterrupt(std::function<void(Edge)> callback) override;
    bool DisableInterrupt() override;

    bool Init(const GpioConfig &config);
    void DeInit();

    std::function<void(Edge)> isrCallback;

  private:
    bool isActiveLow = false;
    uint8_t instanceCount = 0;
    gpio_num_t pinNumber = GPIO_NUM_NC;
};

} // namespace Gpio

#endif /* OSAL_ESP_IDF_GPIO */
