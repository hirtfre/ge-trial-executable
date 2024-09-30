#ifndef OSAL_ESP_IDF_TIMER
#define OSAL_ESP_IDF_TIMER

#include "TimerConfig.hpp"
#include <AbstractionLayer/ITimer.h>

namespace Timer {

class Timer : public ITimer
{
  public:
    Timer() = default;
    ~Timer() override = default;

    Timer &operator=(Timer &&) = delete;

    bool Enable(std::function<void(uint32_t)> callback) override;
    bool Start(uint32_t timeMs) override;
    uint32_t Pause() override;
    uint32_t GetTimerValue() override;

    bool Init(const TimerConfig &config);
    void DeInit();
    std::function<void(uint32_t)> Callback;

    bool isRunning = false;

  private:
    gptimer_handle_t gpTimerHandle;
};

} // namespace Timer

#endif /* OSAL_ESP_IDF_TIMER */
