#include "Task.h"
#include <AbstractionLayer/IMemory.h>
#include <array>
#include <esp_log.h>
#include <new>

namespace Task {

static constexpr uint32_t stackSize =
    4096; // TODO: Adjust stack size back to 2048 and run MatterDataModel calls in a separate task
static const UBaseType_t highestPriority = configMAX_PRIORITIES - 1;
static constexpr std::array<UBaseType_t, 3> taskPriorities = {
    1 /*low*/, highestPriority / 2 /*medium*/, highestPriority /*high*/};

[[noreturn]] static void TaskFunction(void *const context);

ITask *TaskFactory::Create(ITaskAction *taskAction, TaskPriority priority)
{
    Task *task;

    auto memory = MEM_Calloc(1, sizeof(Task));

    if (!memory)
    {
        goto memory_allocation_failed;
    }

    task = new (memory) Task(taskAction);

    if (!task->Init(priority))
    {
        goto task_init_failed;
    }

    return task;

    task->DeInit();

task_init_failed:
    task->~Task();
    MEM_Free(memory);

memory_allocation_failed:
    return nullptr;
}

void TaskFactory::Destroy(ITask **task)
{
    if (*task == nullptr)
    {
        return;
    }

    auto *instance = static_cast<Task *>(*task);

    instance->DeInit();
    instance->~Task();
    MEM_Free(instance);
    *task = nullptr;
}

Task::Task(ITaskAction *taskAction) : taskActionHandle(taskAction) {}

bool Task::Init(TaskPriority priority)
{
    if (xTaskCreate(TaskFunction, "GE Task", stackSize, this,
                    taskPriorities[static_cast<uint8_t>(priority)], &taskHandle) != pdPASS)
    {
        goto task_creation_failed;
    }

    return true;

    vTaskDelete(taskHandle);

task_creation_failed:
    return false;
}

void Task::DeInit()
{
    vTaskDelete(taskHandle);
}

void Task::RunOnce(uint32_t delayMs)
{
    runOnce = true;

    NotifyTask(pdMS_TO_TICKS(delayMs));
}

void Task::RunPeriodically(uint32_t periodMs)
{
    runOnce = false;

    NotifyTask(pdMS_TO_TICKS(periodMs));
}

void Task::Stop()
{
    NotifyTask(portMAX_DELAY);
}

void Task::NotifyTask(TickType_t waitTicks)
{
    BaseType_t higherPriorityTaskWoken = pdFALSE;

    xTaskNotifyFromISR(taskHandle, waitTicks, eSetValueWithOverwrite, &higherPriorityTaskWoken);

    portYIELD_FROM_ISR(higherPriorityTaskWoken);
}

[[noreturn]] static void TaskFunction(void *const context)
{
    auto *task = static_cast<Task *>(context);

    TickType_t nextWaitTicks = 0;
    TickType_t waitTicks = portMAX_DELAY;

    while (true)
    {
        bool timeUpdateReceived = xTaskNotifyWait(0, 0, &nextWaitTicks, waitTicks);

        if (timeUpdateReceived)
        {
            waitTicks = nextWaitTicks;
        }
        else if (waitTicks != portMAX_DELAY)
        {
            task->taskActionHandle->Run(task);

            if (task->runOnce)
            {
                waitTicks = portMAX_DELAY;
            }
        }
    }
}

} // namespace Task