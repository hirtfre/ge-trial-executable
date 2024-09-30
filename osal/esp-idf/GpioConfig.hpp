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

static constexpr std::array<GpioConfig, 10> gpioConfig = {{
    // UpRelay
    {
        .config = 
            {
                .mode = GPIO_MODE_INPUT_OUTPUT,
                .pull_up_en = GPIO_PULLUP_DISABLE,
                .pull_down_en = GPIO_PULLDOWN_ENABLE,
                .intr_type = GPIO_INTR_DISABLE,
                .hys_ctrl_mode = GPIO_HYS_SOFT_DISABLE,
            },
        .pinNumber = GPIO_NUM_11,
        .isActiveLow = false,
    },
    // DownRelay
    {
        .config = 
            {
                .mode = GPIO_MODE_INPUT_OUTPUT,
                .pull_up_en = GPIO_PULLUP_DISABLE,
                .pull_down_en = GPIO_PULLDOWN_ENABLE,
                .intr_type = GPIO_INTR_DISABLE,
                .hys_ctrl_mode = GPIO_HYS_SOFT_DISABLE,
            },
        .pinNumber = GPIO_NUM_12,
        .isActiveLow = false,
    },
    // PowerEnable
    {
        .config = 
            {
                .mode = GPIO_MODE_INPUT_OUTPUT,
                .pull_up_en = GPIO_PULLUP_DISABLE,
                .pull_down_en = GPIO_PULLDOWN_ENABLE,
                .intr_type = GPIO_INTR_DISABLE,
                .hys_ctrl_mode = GPIO_HYS_SOFT_DISABLE,
            },
        .pinNumber = GPIO_NUM_13,
        .isActiveLow = false,
    },
    // MaintainedSwitchUp
    {
        .config = 
            {
                .mode = GPIO_MODE_INPUT,
                .pull_up_en = GPIO_PULLUP_DISABLE,
                .pull_down_en = GPIO_PULLDOWN_DISABLE,
                .intr_type = GPIO_INTR_POSEDGE,
                .hys_ctrl_mode = GPIO_HYS_SOFT_DISABLE,
            },
        .pinNumber = GPIO_NUM_22,
        .isActiveLow = false,
    },
    // MaintainedSwitchDown
    {
        .config = 
            {
                .mode = GPIO_MODE_INPUT,
                .pull_up_en = GPIO_PULLUP_DISABLE,
                .pull_down_en = GPIO_PULLDOWN_DISABLE,
                .intr_type = GPIO_INTR_POSEDGE,
                .hys_ctrl_mode = GPIO_HYS_SOFT_DISABLE,
            },
        .pinNumber = GPIO_NUM_14,
        .isActiveLow = false,
    },
    // nRST
    {
        .config = 
            {
                .mode = GPIO_MODE_OUTPUT,
                .pull_up_en = GPIO_PULLUP_DISABLE,
                .pull_down_en = GPIO_PULLDOWN_DISABLE,
                .intr_type = GPIO_INTR_DISABLE,
                .hys_ctrl_mode = GPIO_HYS_SOFT_DISABLE,
            },
        .pinNumber = GPIO_NUM_9,
        .isActiveLow = false,
    },
    // ZX
    {
        .config = 
            {
                .mode = GPIO_MODE_INPUT,
                .pull_up_en = GPIO_PULLUP_DISABLE,
                .pull_down_en = GPIO_PULLDOWN_DISABLE,
                .intr_type = GPIO_INTR_ANYEDGE,
                .hys_ctrl_mode = GPIO_HYS_SOFT_DISABLE,
            },
        .pinNumber = GPIO_NUM_10,
        .isActiveLow = false,
    },
    // ZX_I
    {
        .config = 
            {
                .mode = GPIO_MODE_INPUT,
                .pull_up_en = GPIO_PULLUP_DISABLE,
                .pull_down_en = GPIO_PULLDOWN_DISABLE,
                .intr_type = GPIO_INTR_POSEDGE,
                .hys_ctrl_mode = GPIO_HYS_SOFT_DISABLE,
            },
        .pinNumber = GPIO_NUM_3,
        .isActiveLow = false,
    },
    // nREVP
    {
        .config = 
            {
                .mode = GPIO_MODE_INPUT,
                .pull_up_en = GPIO_PULLUP_DISABLE,
                .pull_down_en = GPIO_PULLDOWN_DISABLE,
                .intr_type = GPIO_INTR_NEGEDGE,
                .hys_ctrl_mode = GPIO_HYS_SOFT_DISABLE,
            },
        .pinNumber = GPIO_NUM_8,
        .isActiveLow = false,
    },
    // nIRQ
    {
        .config = 
            {
                .mode = GPIO_MODE_INPUT,
                .pull_up_en = GPIO_PULLUP_DISABLE,
                .pull_down_en = GPIO_PULLDOWN_DISABLE,
                .intr_type = GPIO_INTR_NEGEDGE,
                .hys_ctrl_mode = GPIO_HYS_SOFT_DISABLE,
            },
        .pinNumber = GPIO_NUM_2,
        .isActiveLow = false,
    },
}};

enum class Type : uint8_t
{
    UpRelay = 0,
    DownRelay = 1,
    PowerEnable = 2,
    MaintainedSwitchUp = 3,
    MaintainedSwitchDown = 4,
    nRST = 5,
    ZX = 6,
    ZX_I = 7,
    nREVP = 8,
    nIRQ = 9,
};

} // namespace Gpio

#endif /* CONFIG_GPIO_CONFIG_H */
