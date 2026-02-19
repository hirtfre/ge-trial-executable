#include "Task.h"
#include <cstring>
#include <utility>

namespace Task {

static void SetNameBuffer(std::array<char, configMAX_TASK_NAME_LEN> &buffer, std::string_view name);

bool Task::Init(void (*taskFunction)(void *), Priority priority, uint16_t stackDepth,
                std::string_view name)
{
    std::array<char, configMAX_TASK_NAME_LEN> nameBuffer = {};
    SetNameBuffer(nameBuffer, name);

    if (xTaskCreate(taskFunction, nameBuffer.data(), stackDepth, this,
                    taskPriorities[std::to_underlying(priority)], &taskHandle) != pdPASS)
    {
        goto task_init_failed;
    }

    return true;

    vTaskDelete(taskHandle);

task_init_failed:
    return false;
}

void Task::DeInit()
{
    vTaskDelete(taskHandle);
}

static void SetNameBuffer(std::array<char, configMAX_TASK_NAME_LEN> &buffer, std::string_view name)
{
    const size_t length = (std::min)(name.size(), static_cast<size_t>(configMAX_TASK_NAME_LEN - 1));
    std::memcpy(buffer.data(), name.data(), length);
    buffer[length] = '\0';
}

} // namespace Task