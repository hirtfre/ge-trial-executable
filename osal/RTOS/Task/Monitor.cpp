#include "Monitor.h"
#include <Trace/ITrace.h>
#include <array>

namespace Task::Monitor {

#ifdef ENABLE_MONITOR

struct TaskInfo
{
    TaskHandle_t taskHandle;   ///< Handle to the FreeRTOS task.
    uint16_t maxStackDepth;    ///< The total stack depth allocated for the task in words.
    QueueHandle_t queueHandle; ///< (Optional) Queue handle the task uses.
    uint8_t maxQueueLength;    ///< Maximum length of the queue.
    uint8_t maxElementsQueued; ///< Peak number of elements that have been in the queue.
};

static constexpr const char *moduleName = "Osal.Task.Mon";
static constexpr uint8_t maxMonitoredTasks = 16;

static std::array<TaskInfo, maxMonitoredTasks> monitoredTasks = {};
static uint8_t monitoredTasksCount = 0;
static bool hasMonitorStarted = false;

static void MonitorTask(void *pvParameters);
static inline void LogStackUsage(const TaskInfo &monitoredTask, const TaskStatus_t &taskStatus);
static inline void LogQueueUsage(const TaskInfo &monitoredTask, const char *taskName);

void Register(TaskHandle_t taskHandle, uint16_t maxStackDepth, QueueHandle_t queueHandle,
              uint8_t maxQueueLength)
{
    if (monitoredTasksCount >= maxMonitoredTasks)
    {
        Trace::Log(moduleName, Trace::Level::Error,
                   "Failed to register task increase number of monitorable tasks (currently %d)",
                   maxMonitoredTasks);
        return;
    }

    auto &slot = monitoredTasks[monitoredTasksCount++];
    slot.taskHandle = taskHandle;
    slot.maxStackDepth = maxStackDepth;

    if (queueHandle != nullptr)
    {
        slot.queueHandle = queueHandle;
        slot.maxQueueLength = maxQueueLength;
    }

    if (!hasMonitorStarted)
    {
        xTaskCreate(MonitorTask, "Ge.Osal.TskMon", 4096, nullptr, 1, nullptr);
        hasMonitorStarted = true;
    }
}

void MonitorQueueUsage(TaskHandle_t taskHandle)
{
    for (auto &taskInfo : monitoredTasks)
    {
        if (taskInfo.taskHandle == taskHandle && taskInfo.queueHandle != nullptr)
        {
            uint8_t elementsQueued = uxQueueMessagesWaiting(taskInfo.queueHandle);
            taskInfo.maxElementsQueued = std::max(elementsQueued, taskInfo.maxElementsQueued);
            break;
        }
    }
}

static void MonitorTask([[maybe_unused]] void *pvParameters)
{
    while (true)
    {
        vTaskDelay(pdMS_TO_TICKS(60000));

        UBaseType_t totalTasks = uxTaskGetNumberOfTasks();
        TaskStatus_t *taskStatusArray =
            (TaskStatus_t *)pvPortMalloc(totalTasks * sizeof(TaskStatus_t));

        if (taskStatusArray == nullptr)
        {
            Trace::Log(moduleName, Trace::Level::Error,
                       "Failed to allocate memory for task status array");
            continue;
        }

        UBaseType_t arraySize = uxTaskGetSystemState(taskStatusArray, totalTasks, nullptr);

        for (auto &monitoredTask : monitoredTasks)
        {
            for (UBaseType_t j = 0; j < arraySize; j++)
            {
                if (taskStatusArray[j].xHandle == monitoredTask.taskHandle)
                {
                    LogStackUsage(monitoredTask, taskStatusArray[j]);
                    LogQueueUsage(monitoredTask, taskStatusArray[j].pcTaskName);
                    break;
                }
            }
        }

        vPortFree(taskStatusArray);
    }
}

static void LogStackUsage(const TaskInfo &monitoredTask, const TaskStatus_t &taskStatus)
{
    const uint16_t usedStack = monitoredTask.maxStackDepth - taskStatus.usStackHighWaterMark;
    const uint16_t threshold = (2 * monitoredTask.maxStackDepth) / 3;

    if (usedStack > threshold)
    {
        const uint16_t recommendedStackDepth = ((usedStack * 3) + 1) / 2;
        Trace::Log(moduleName, Trace::Level::Warning,
                   "Task '%s' stack usage is %u / %u words. Increase stack to at least %u words.",
                   taskStatus.pcTaskName, usedStack, monitoredTask.maxStackDepth,
                   recommendedStackDepth);
    }
    else
    {
        Trace::Log(moduleName, Trace::Level::Info, "Task '%s' stack usage: %u / %u words.",
                   taskStatus.pcTaskName, usedStack, monitoredTask.maxStackDepth);
    }
}

static void LogQueueUsage(const TaskInfo &monitoredTask, const char *taskName)
{
    if (monitoredTask.queueHandle == nullptr)
    {
        return;
    }

    const uint8_t peakUsage = monitoredTask.maxElementsQueued;
    const uint16_t threshold = (2 * monitoredTask.maxQueueLength) / 3;

    if (peakUsage > threshold)
    {
        const uint16_t recommendedLength = ((peakUsage * 3) + 1) / 2;
        Trace::Log(moduleName, Trace::Level::Warning,
                   "Task '%s' queue usage is %u / %u. Increase queue length to at least %u.",
                   taskName, peakUsage, monitoredTask.maxQueueLength, recommendedLength);
    }
    else
    {
        Trace::Log(moduleName, Trace::Level::Info, "Task '%s' queue usage: %u / %u.", taskName,
                   peakUsage, monitoredTask.maxQueueLength);
    }
}

#endif // ENABLE_MONITOR

} // namespace Task::Monitor
