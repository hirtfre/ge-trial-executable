#ifndef OSAL_FREE_RTOS_TASK
#define OSAL_FREE_RTOS_TASK

#include <AbstractionLayer/Task/ITask.h>
#include <AbstractionLayer/Task/ITaskFactory.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace Task {

class Task : public ITask
{
  public:
    explicit Task(ITaskAction *taskAction);
    ~Task() override = default;

    Task &operator=(Task &&) = delete;

    bool Init(TaskPriority priority);
    void DeInit();

    void RunOnce(uint32_t delayInMs) override;
    void RunPeriodically(uint32_t periodInMs) override;
    void Stop() override;

    bool runOnce = true;
    ITaskAction *taskActionHandle;

  private:
    void NotifyTask(TickType_t waitTicks);
    TaskHandle_t taskHandle;
};

} // namespace Task

#endif /* OSAL_FREE_RTOS_TASK */
