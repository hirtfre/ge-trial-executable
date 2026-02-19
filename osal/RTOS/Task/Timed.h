#ifndef OSAL_RTOS_TASK_TIMED
#define OSAL_RTOS_TASK_TIMED

#include "Task.h"
#include <AbstractionLayer/Task/ITaskFactory.h>
#include <AbstractionLayer/Task/ITimed.h>

namespace Task {

class Timed : public Task, public ITimed
{
  public:
    explicit Timed(TimedAction pAction) : action(pAction) {};
    ~Timed() override = default;

    Timed(const Timed &) = delete;
    Timed &operator=(const Timed &) = delete;
    Timed(Timed &&) = delete;
    Timed &operator=(Timed &&) = delete;

    void RunOnce(uint32_t delayInMs) override;
    void RunPeriodically(uint32_t periodInMs) override;
    void Stop() override;

    void RunOnceFromIsr(uint32_t delayInMs) override;
    void RunPeriodicallyFromIsr(uint32_t periodInMs) override;
    void StopFromIsr() override;

    TimedAction action = nullptr;
    bool runOnce : 1 = true;

  private:
    void NotifyTask(TickType_t waitTicks);
    void NotifyTaskFromIsr(TickType_t waitTicks);
};

} // namespace Task

#endif /* OSAL_RTOS_TASK_TIMED */
