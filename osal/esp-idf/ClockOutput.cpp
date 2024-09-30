#include "ClockOutput.h"
#include <AbstractionLayer/IClockOutputFactory.h>
#include <AbstractionLayer/IMemory.h>
#include <algorithm>
#include <array>
#include <cstring>
#include <new>

namespace ClockOutput {

ClockOutput::~ClockOutput()
{
    DeInit();
}

bool ClockOutput::Init(const ClockOutputConfig &config)
{
    ledc_timer_config_t timerConfig;
    std::memcpy(&timerConfig, &config.timerConfig, sizeof(ledc_timer_config_t));

    if (ledc_timer_config(&timerConfig) != ESP_OK)
    {
        goto unable_to_configure_timer;
    }

    ledc_channel_config_t channelConfig;
    std::memcpy(&channelConfig, &config.channelConfig, sizeof(ledc_channel_config_t));

    if (ledc_channel_config(&channelConfig) != ESP_OK)
    {
        goto unable_to_configure_channel;
    }

    channel = config.channelConfig.channel;
    speedMode = config.channelConfig.speed_mode;
    timerNum = timerConfig.timer_num;

    return true;

unable_to_configure_channel:
    ledc_timer_pause(timerConfig.speed_mode, timerConfig.timer_num);
    timerConfig.deconfigure = true;
    ledc_timer_config(&timerConfig);

unable_to_configure_timer:

    return false;
}

void ClockOutput::DeInit() const
{
    // there is no way to properly deinitialize led_channel
    // so simply stop it
    ledc_stop(speedMode, channel, 0);

    ledc_timer_pause(speedMode, timerNum);

    ledc_timer_config_t timerConfig;
    timerConfig.deconfigure = true;
    ledc_timer_config(&timerConfig);
}

bool ClockOutput::Start()
{
    if (ledc_set_duty(speedMode, channel, 1) != ESP_OK)
    {
        return false;
    }

    if (ledc_update_duty(speedMode, channel) != ESP_OK)
    {
        return false;
    }

    return true;
}

bool ClockOutput::Stop()
{
    ledc_stop(speedMode, channel, 0);
    return true;
}

static std::array<ClockOutput *, clockOutputConfig.size()> clockOutputList{};

IClockOutput *ClockOutputFactory::Create(Type type)
{
    ClockOutput *clockOutput = nullptr;
    auto index = static_cast<uint8_t>(type);

    clockOutput = clockOutputList[index];

    if (clockOutput != nullptr)
    {
        return clockOutput;
    }

    auto memory = MEM_Calloc(1, sizeof(ClockOutput));

    if (memory == nullptr)
    {
        goto unable_to_allocate_memory;
    }

    clockOutput = new (memory) ClockOutput();

    if (!clockOutput->Init(clockOutputConfig[index]))
    {
        goto unable_to_initialize_clock;
    }

    clockOutputList[index] = clockOutput;

    return clockOutput;

unable_to_initialize_clock:
    MEM_Free(memory);

unable_to_allocate_memory:
    return nullptr;
}

void ClockOutputFactory::Destroy(IClockOutput **instance)
{
    if (instance == nullptr || *instance == nullptr)
    {
        return;
    }

    auto *clockOutput = static_cast<ClockOutput *>(*instance);
    std::ranges::replace(clockOutputList, clockOutput, nullptr);

    clockOutput->~ClockOutput();
    MEM_Free(*instance);

    *instance = nullptr;
    instance = nullptr;
}

} // namespace ClockOutput