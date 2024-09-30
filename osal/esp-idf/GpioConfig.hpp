#ifndef CONFIG_GPIO_CONFIG_H
#define CONFIG_GPIO_CONFIG_H

#include <array>
#include <driver/gpio.h>

/********************************
* Auto generated file
* DO NOT EDIT
*********************************/

namespace Gpio {

struct GpioConfig
{
    gpio_config_t config;
    gpio_num_t pinNumber;
    bool isActiveLow;
};

static constexpr std::array<GpioConfig, 0> gpioConfig;

enum class Type : uint8_t
{
    None,
};

} // namespace Gpio

#endif /* CONFIG_GPIO_CONFIG_H */
