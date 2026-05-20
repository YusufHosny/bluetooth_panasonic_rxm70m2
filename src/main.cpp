#include "esp_log.h"
#include "nvs_flash.h"

#include "bt_audio.h"
#include "buttons.h"

static const char * TAG = "main";

extern "C" void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_LOGI(TAG, "starting");
    bt_audio_init();
    buttons_init();
    ESP_LOGI(TAG, "ready");
}
