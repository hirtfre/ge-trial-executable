#ifndef CONFIG_CLOCK_OUTPUT_CONFIG_H
#define CONFIG_CLOCK_OUTPUT_CONFIG_H

#include <array>
#include <driver/ledc.h>

/********************************
* Auto generated file
* DO NOT EDIT
*********************************/

namespace ClockOutput {

struct ClockOutputConfig
{
    ledc_timer_config_t timerConfig;
    ledc_channel_config_t channelConfig;
};

static constexpr std::array<ClockOutputConfig, 1> clockOutputConfig = {{
    // ADE7953
    {
        .timerConfig = 
            {
                .speed_mode = LEDC_LOW_SPEED_MODE,
                .duty_resolution = LEDC_TIMER_1_BIT,
                .timer_num = LEDC_TIMER_0,
                .freq_hz = 3580000,
                .clk_cfg = LEDC_USE_PLL_DIV_CLK,
            },
        .channelConfig =
            {
                .gpio_num = 25,
                .speed_mode = LEDC_LOW_SPEED_MODE,
                .channel = LEDC_CHANNEL_0,
                .intr_type = LEDC_INTR_DISABLE,
                .timer_sel = LEDC_TIMER_0,
                .duty = 0,
                .hpoint = 0,
            },
    },
}};

enum class Type : uint8_t
{
    ADE7953 = 0,
};

} // namespace ClockOutput

#endif /* CONFIG_CLOCK_OUTPUT_CONFIG_H */
