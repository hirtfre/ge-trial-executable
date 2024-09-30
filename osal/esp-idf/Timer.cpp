#include "Timer.h"
#include "esp_attr.h"
#include "esp_timer.h"
#include <AbstractionLayer/ITimerFactory.h>
#include <array>
#include <cstdlib>

namespace Timer {

int64_t ITimer::GetTimeSinceBootMs()
{
    return esp_timer_get_time() / 1000;
}

bool ITimer::HasTimePassed(int64_t startTimeMs, int64_t timeMs)
{
    return GetTimeSinceBootMs() - startTimeMs >= timeMs;
}

static bool IRAM_ATTR timerCallback(gptimer_handle_t handle,
                                    const gptimer_alarm_event_data_t *edata, void *me);

static std::array<Timer, timerConfig.size()> timerList;

ITimer *TimerFactory::Create(Type type)
{
    auto index = static_cast<uint8_t>(type);
    auto *instance = &timerList[index];

    if (instance->Init(timerConfig[index]))
    {
        return nullptr;
    }

    return instance;
}

void TimerFactory::Destroy(ITimer **timer)
{
    auto *instance = static_cast<Timer *>(*timer);

    instance->DeInit();
    *timer = nullptr;
}

bool Timer::Init(const TimerConfig &config)
{
    esp_err_t err = gptimer_new_timer(&config.config, &gpTimerHandle);

    isRunning = false;

    return (err != ESP_OK);
}

void Timer::DeInit()
{
    (void)gptimer_disable(gpTimerHandle);
    (void)gptimer_del_timer(gpTimerHandle);

    isRunning = false;
}

bool Timer::Enable(std::function<void(uint32_t)> pCallback)
{
    Callback = pCallback;

    gptimer_event_callbacks_t callbackService = {
        .on_alarm = timerCallback,
    };
    esp_err_t err = gptimer_register_event_callbacks(gpTimerHandle, &callbackService, this);

    err |= gptimer_enable(gpTimerHandle);

    return (err != ESP_OK);
}

bool Timer::Start(uint32_t timeMs)
{
    esp_err_t err = gptimer_set_raw_count(gpTimerHandle, 0);

    gptimer_alarm_config_t alarmConfig = {};
    alarmConfig.alarm_count = timeMs * 1000;

    err |= gptimer_set_alarm_action(gpTimerHandle, &alarmConfig);
    err |= gptimer_start(gpTimerHandle);

    isRunning = err == ESP_OK;

    return isRunning;
}

uint32_t Timer::Pause()
{
    uint32_t timePasedMs = GetTimerValue();

    if (isRunning)
    {
        isRunning = false;
        (void)gptimer_stop(gpTimerHandle);
    }

    return timePasedMs;
}

uint32_t Timer::GetTimerValue()
{
    uint64_t count;

    if (gptimer_get_raw_count(gpTimerHandle, &count) != ESP_OK)
    {
        return UINT32_MAX;
    }

    return static_cast<uint32_t>(count / 1000);
}

static bool timerCallback(gptimer_handle_t handle, const gptimer_alarm_event_data_t *edata,
                          void *me)
{
    auto *timer = static_cast<Timer *>(me);
    timer->isRunning = false;
    (void)gptimer_stop(handle);
    timer->Callback(static_cast<uint32_t>(edata->count_value / 1000));

    return true;
}

} // namespace Timer