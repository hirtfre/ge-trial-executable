#ifndef OSAL_RTOS_TASK_MONITOR
#define OSAL_RTOS_TASK_MONITOR

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>

/* 
 * Uncomment this to enable the task Monitor
 */
//#define ENABLE_MONITOR

#ifdef NDEBUG
#ifdef ENABLE_MONITOR
#undef ENABLE_MONITOR
#endif
#endif

namespace Task::Monitor {

#ifndef ENABLE_MONITOR

/**
 * @brief No-op when monitoring is disabled.
 */
static inline void Register(TaskHandle_t, uint16_t,
                            [[maybe_unused]] QueueHandle_t queueHandle = nullptr,
                            [[maybe_unused]] uint8_t maxQueueLength = 0)
{
    // Do nothing
}

/**
 * @brief No-op when monitoring is disabled.
 */
static inline void MonitorQueueUsage(TaskHandle_t)
{
    // Do nothing
}

#else // ENABLE_MONITOR is defined

/**
 * @brief Registers a task for monitoring.
 * 
 * Starts the monitor task if it has not already been started.
 * 
 * @param taskHandle     Handle to the FreeRTOS task to monitor.
 * @param maxStackDepth  The total stack depth allocated for the task in words.
 * @param queueHandle    (Optional) Queue handle used by this task, or nullptr if none.
 * @param maxQueueLength (Optional) Capacity of the queue in items (0 if no queue).
 */
void Register(TaskHandle_t taskHandle, uint16_t maxStackDepth, QueueHandle_t queueHandle = nullptr,
              uint8_t maxQueueLength = 0);

/**
 * @brief Updates peak queue usage for the specified task.
 *
 * Call this before a task dequeues an item.
 * 
 * @param taskHandle  Handle of the task which the queue belongs to.
 */
void MonitorQueueUsage(TaskHandle_t taskHandle);

#endif // ENABLE_MONITOR

} // namespace Task::Monitor

#endif /* OSAL_RTOS_TASK_MONITOR */
