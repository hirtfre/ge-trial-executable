#ifndef CONFIG_TIMER_CONFIG_H
#define CONFIG_TIMER_CONFIG_H

#include "driver/gptimer.h"
#include <array>

/********************************
* Auto generated file
* DO NOT EDIT
*********************************/

namespace Timer {

struct TimerConfig
{
    gptimer_config_t config;
};
// clang-format off
static constexpr std::array<TimerConfig, 0> timerConfig;

enum class Type : uint8_t
{
    None,
};
// clang-format on

} // namespace Timer

#endif /* CONFIG_TIMER_CONFIG_H */
