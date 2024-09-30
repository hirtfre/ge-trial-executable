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
static constexpr std::array<TimerConfig, 1> timerConfig = {{
    // MotorRuntime
    {
        .config    
            {
                .clk_src = GPTIMER_CLK_SRC_DEFAULT,
                .direction = GPTIMER_COUNT_UP,
                .resolution_hz = 1000000,
                .intr_priority = 3,
                .flags = 0,
            }
    },
}};

enum class Type : uint8_t
{
    };
// clang-format on

} // namespace Timer

#endif /* CONFIG_TIMER_CONFIG_H */
