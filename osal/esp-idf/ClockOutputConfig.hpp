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

static constexpr std::array<ClockOutputConfig, 0> clockOutputConfig;

enum class Type : uint8_t
{
    None,
};

} // namespace ClockOutput

#endif /* CONFIG_CLOCK_OUTPUT_CONFIG_H */
