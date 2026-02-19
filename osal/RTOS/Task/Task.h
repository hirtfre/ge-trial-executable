#ifndef OSAL_RTOS_TASK_TASK
#define OSAL_RTOS_TASK_TASK

#include <AbstractionLayer/Task/ITaskFactory.h>
#include <Trace/ITrace.h>
#include <array>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <string_view>

namespace Task {

static constexpr const char *moduleName = "Osal.Task";

static_assert(configMAX_PRIORITIES >= 4,
              "We require at least 4 FreeRTOS priorities (configMAX_PRIORITIES >= 4).");

constexpr UBaseType_t highestPriority = configMAX_PRIORITIES - 1;

constexpr std::array<UBaseType_t, 4> taskPriorities = {
    1,                   // Low
    highestPriority / 2, // Medium
    highestPriority - 1, // High
    highestPriority,     // RealTime
};

class Task : public ITask
{
  public:
    Task() = default;
    ~Task() override = default;

    Task(const Task &) = delete;
    Task &operator=(const Task &) = delete;
    Task(Task &&) = delete;
    Task &operator=(Task &&) = delete;

    virtual bool Init(void (*taskFunction)(void *), Priority priority, uint16_t stackDepth,
                      std::string_view name);
    virtual void DeInit();

    TaskHandle_t taskHandle;
};

} // namespace Task

#endif /* OSAL_RTOS_TASK_TASK */
