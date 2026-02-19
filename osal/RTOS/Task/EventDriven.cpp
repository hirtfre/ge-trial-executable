#include "EventDriven.h"
#include "Monitor.h"
#include <AbstractionLayer/IMemory.h>
#include <cstring>
#include <new>

namespace Task {

[[noreturn]] static void EventDrivenTaskFunction(void *const context);

IEventDriven *Factory::CreateEventDriven(EventAction action, Priority priority, uint16_t stackDepth,
                                         uint8_t queueLength, uint8_t eventSize,
                                         std::string_view name)
{
    EventDriven *task = nullptr;

    auto memory = MEM_Calloc(1, sizeof(EventDriven));

    if (memory == nullptr)
    {
        goto memory_allocation_failed;
    }

    task = new (memory) EventDriven(action, eventSize);

    if (!task->InitQueue(queueLength, eventSize))
    {
        goto queue_init_failed;
    }

    if (!task->Task::Init(EventDrivenTaskFunction, priority, stackDepth, name))
    {
        goto task_init_failed;
    }

    Monitor::Register(task->taskHandle, stackDepth, task->queue, queueLength);

    return task;

    task->Task::DeInit();

task_init_failed:
    task->DeInitQueue();

queue_init_failed:
    task->~EventDriven();
    MEM_Free(memory);

memory_allocation_failed:
    Trace::Log(moduleName, Trace::Level::Error, "Creation of event driven task %s failed.",
               name.data());
    return nullptr;
}

void Factory::DestroyEventDriven(IEventDriven **task)
{
    if (task == nullptr || *task == nullptr)
    {
        return;
    }

    auto *instance = static_cast<EventDriven *>(*task);

    instance->DeInitQueue();
    instance->Task::DeInit();
    instance->~EventDriven();
    MEM_Free(instance);
    *task = nullptr;
}

bool EventDriven::InitQueue(uint8_t queueLength, uint8_t elementSize)
{
    queue = xQueueCreate(queueLength, elementSize);

    if (queue == nullptr)
    {
        goto queue_creation_failed;
    }

    return true;

    vQueueDelete(queue);

queue_creation_failed:
    return false;
}

void EventDriven::DeInitQueue()
{
    vQueueDelete(queue);
}

void EventDriven::Send(const IEvent &item)
{
    (void)xQueueSend(queue, &item, portMAX_DELAY);
}

void EventDriven::SendFromIsr(const IEvent &item)
{
    auto hasHigherPriorityTaskWoken = pdFALSE;
    xQueueSendFromISR(queue, &item, &hasHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(hasHigherPriorityTaskWoken);
}

[[noreturn]] static void EventDrivenTaskFunction(void *const context)
{
    auto *task = static_cast<EventDriven *>(context);
    uint8_t elementBuffer[task->eventSize] = {};

    while (true)
    {
        Monitor::MonitorQueueUsage(task->taskHandle);

        if (xQueueReceive(task->queue, &elementBuffer, portMAX_DELAY) == pdTRUE)
        {
            auto *event = reinterpret_cast<IEvent *>(&elementBuffer);
            task->action(*event);
        }
    }
}

} // namespace Task