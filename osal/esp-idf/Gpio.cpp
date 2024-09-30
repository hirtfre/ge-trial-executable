#include "Gpio.h"
#include "GpioConfig.hpp"
#include "esp_attr.h"
#include <AbstractionLayer/IGpioFactory.h>
#include <array>
#include <cstdint>
#include <cstring>

namespace Gpio {

static void IRAM_ATTR isrHandler(void *arg);

static std::array<Gpio, gpioConfig.size()> gpioList;
static bool isIsrServiceInstalled = false;

IGpio *GpioFactory::Create(Type type)
{
    auto index = static_cast<uint8_t>(type);
    Gpio *instance = &gpioList[index];

    if (instance->Init(gpioConfig[index]))
    {
        return nullptr;
    }

    instance->Off();

    return instance;
}

void GpioFactory::Destroy(IGpio **gpio)
{
    auto *instance = (Gpio *)(*gpio);

    instance->DeInit();
    *gpio = nullptr;
}

bool Gpio::Read()
{
    return gpio_get_level(pinNumber);
}

bool Gpio::Write(bool value)
{
    return gpio_set_level(pinNumber, value) == ESP_OK;
}

bool Gpio::On()
{
    return Write(!isActiveLow);
}

bool Gpio::Off()
{
    return Write(isActiveLow);
}

bool Gpio::Init(const GpioConfig &config)
{
    gpio_config_t configToSet;

    memcpy(&configToSet, &config.config, sizeof(gpio_config_t));

    pinNumber = config.pinNumber;
    configToSet.pin_bit_mask = (1ULL << (config.pinNumber));
    isActiveLow = config.isActiveLow;

    return gpio_config(&configToSet) != ESP_OK;
}

void Gpio::DeInit()
{
    instanceCount--;

    if (instanceCount == 0)
    {
        gpio_reset_pin(pinNumber);
        pinNumber = GPIO_NUM_NC;
    }
}

bool Gpio::EnableInterrupt(std::function<void(Edge)> callback)
{
    if (!isIsrServiceInstalled)
    {
        gpio_install_isr_service(0);
        isIsrServiceInstalled = true;
    }

    if (gpio_isr_handler_add(pinNumber, isrHandler, (void *)this) != ESP_OK)
    {
        return false;
    }

    isrCallback = callback;

    return true;
}

bool Gpio::DisableInterrupt()
{
    if (gpio_isr_handler_remove(pinNumber) != ESP_OK)
    {
        return false;
    }

    isrCallback = nullptr;
    return true;
}

static void isrHandler(void *arg)
{
    auto gpio = (Gpio *)(arg);

    if (gpio->isrCallback != nullptr)
    {
        Edge edge = gpio->Read() ? Edge::Rising : Edge::Falling;
        gpio->isrCallback(edge);
    }
}

} // namespace Gpio