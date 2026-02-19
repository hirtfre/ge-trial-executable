#include "Timed.h"
#include "Monitor.h"
#include <AbstractionLayer/IMemory.h>
#include <new>

namespace Task {

[[noreturn]] static void TimedTaskFunction(void *const context);

ITimed *Factory::CreateTimed(TimedAction action, Priority priority, uint16_t stackDepth,
                             std::string_view name)
{
    Timed *task = nullptr;

    auto memory = MEM_Calloc(1, sizeof(Timed));

    if (memory == nullptr)
    {
        goto memory_allocation_failed;
    }

    task = new (memory) Timed(action);

    if (!task->Task::Init(&TimedTaskFunction, priority, stackDepth, name))
    {
        goto task_init_failed;
    }

    Monitor::Register(task->taskHandle, stackDepth);

    return task;

    task->DeInit();

task_init_failed:
    task->~Timed();
    MEM_Free(memory);

memory_allocation_failed:
    Trace::Log(moduleName, Trace::Level::Error, "Creation of timed task %s failed", name.data());
    return nullptr;
}

void Factory::DestroyTimed(ITimed **task)
{
    if (task == nullptr || *task == nullptr)
    {
        return;
    }

    auto *instance = static_cast<Timed *>(*task);

    instance->Task::DeInit();
    instance->~Timed();
    MEM_Free(instance);
    *task = nullptr;
}

void Timed::RunOnce(uint32_t delayMs)
{
    runOnce = true;

    NotifyTask(pdMS_TO_TICKS(delayMs));
}

void Timed::RunPeriodically(uint32_t periodMs)
{
    runOnce = false;

    NotifyTask(pdMS_TO_TICKS(periodMs));
}

void Timed::Stop()
{
    NotifyTask(portMAX_DELAY);
}

void Timed::RunOnceFromIsr(uint32_t delayMs)
{
    runOnce = true;

    NotifyTaskFromIsr(pdMS_TO_TICKS(delayMs));
}

void Timed::RunPeriodicallyFromIsr(uint32_t periodMs)
{
    runOnce = false;

    NotifyTaskFromIsr(pdMS_TO_TICKS(periodMs));
}

void Timed::StopFromIsr()
{
    NotifyTaskFromIsr(portMAX_DELAY);
}

void Timed::NotifyTask(TickType_t waitTicks)
{
    (void)xTaskNotify(taskHandle, waitTicks, eSetValueWithOverwrite);
}

void Timed::NotifyTaskFromIsr(TickType_t waitTicks)
{
    auto hasHigherPriorityTaskWoken = pdFALSE;
    (void)xTaskNotifyFromISR(taskHandle, waitTicks, eSetValueWithOverwrite,
                             &hasHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(hasHigherPriorityTaskWoken);
}

[[noreturn]] static void TimedTaskFunction(void *const context)
{
    auto *task = static_cast<Timed *>(context);

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
            task->action();

            if (task->runOnce)
            {
                waitTicks = portMAX_DELAY;
            }
        }
    }
}

} // namespace Task