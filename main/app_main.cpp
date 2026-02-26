#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <AbstractionLayer/Task/ITaskFactory.h>
#include <Trace/ITrace.h>

#include <stdlib.h>
#include <string>
#include <RingBuffer/Factory.h>

static constexpr const char *moduleName = "main";

#define NB_ELEMENT 10
RingBuffer::IRingBuffer *rb = RingBuffer::Factory::Create(NB_ELEMENT);


void push_rb(void) 
{
    uint16_t rand = std::rand();

    rb->Add(rand);
    Trace::Log(moduleName, Trace::Level::Info, "Push RB");
}

void log_rb(void)
{
    while (!rb->IsEmpty())
    {
        uint16_t value;
        rb->Remove(value);
        Trace::Log(moduleName, Trace::Level::Info, "val: %d", value);
    }

}

extern "C" void app_main()
{
    Trace::Log(moduleName, Trace::Level::Info, "Hello World!");

    auto timedTask =
        Task::Factory::CreateTimed(push_rb,
                                   Task::Priority::Medium, 2048, "RB-Push");
    if (timedTask == nullptr)
    {
        Trace::Log(moduleName, Trace::Level::Error, "Task failed");
        return;
    }

    auto timedTaskLog =
        Task::Factory::CreateTimed(log_rb,
                                   Task::Priority::Medium, 2048, "RB-Log");

    if (timedTaskLog == nullptr)
    {
        Trace::Log(moduleName, Trace::Level::Error, "Task 2 failed");
        return;
    }

    timedTask->RunPeriodically(500);
    timedTaskLog->RunPeriodically(1200);

    while (true)
    {
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}