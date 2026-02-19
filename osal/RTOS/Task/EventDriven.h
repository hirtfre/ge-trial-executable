#ifndef OSAL_RTOS_TASK_EVENT_DRIVEN
#define OSAL_RTOS_TASK_EVENT_DRIVEN

#include "Task.h"
#include <AbstractionLayer/Task/IEventDriven.h>
#include <AbstractionLayer/Task/ITaskFactory.h>
#include <freertos/queue.h>

namespace Task {

class EventDriven : public Task, public IEventDriven
{
  public:
    explicit EventDriven(EventAction pAction, uint8_t elementSize) :
        action(pAction), eventSize(elementSize) {};
    ~EventDriven() override = default;

    EventDriven(const EventDriven &) = delete;
    EventDriven &operator=(const EventDriven &) = delete;
    EventDriven(EventDriven &&) = delete;
    EventDriven &operator=(EventDriven &&) = delete;

    bool InitQueue(uint8_t queueLenght, uint8_t elementSize);
    void DeInitQueue();

    void Send(const IEvent &item) override;
    void SendFromIsr(const IEvent &item) override;

    EventAction action = nullptr;
    QueueHandle_t queue = nullptr;
    const uint8_t eventSize;
};

} // namespace Task

#endif /* OSAL_RTOS_TASK_EVENT_DRIVEN */
