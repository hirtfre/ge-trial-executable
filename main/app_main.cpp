#include <esp_log.h>

static const char *TAG = "app_main";

extern "C" void app_main()
{
    ESP_LOGI(TAG, "Hello World!");
}
