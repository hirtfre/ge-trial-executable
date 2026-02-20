#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <AbstractionLayer/Task/ITaskFactory.h>
#include <Trace/ITrace.h>

static constexpr const char *moduleName = "main";

extern "C" void app_main()
{
    Trace::Log(moduleName, Trace::Level::Info, "Hello World!");

    while (true)
    {
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}