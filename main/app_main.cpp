#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <AbstractionLayer/Task/ITaskFactory.h>
#include <Trace/ITrace.h>

static constexpr const char *moduleName = "main";

extern "C" void app_main()
{
    Trace::Log(moduleName, Trace::Level::Info, "Hello World!");

    auto timedTask =
        Task::Factory::CreateTimed([]() { Trace::Log(moduleName, Trace::Level::Info, "Task fired"); },
                                   Task::Priority::Medium, 2048, "RB-Push");
    if (timedTask == nullptr)
    {
        Trace::Log(moduleName, Trace::Level::Error, "Task failed");
        return;
    }

    timedTask->RunPeriodically(1000);

    while (true)
    {
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}