#include <stdint.h>

#include "esp_log.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "config.h"
#include "buttons.h"
#include "bt_audio.h"

static const char * TAG = "buttons";

typedef struct
{
    int pin;
} btn_event_t;

static QueueHandle_t s_btn_queue;

static const struct
{
    int         pin;
    void        (*action)(void);
} btn_map[] = {
    { BTN_PAUSE_PIN,  bt_audio_toggle_pause  },
    { BTN_NEXT_PIN,   bt_audio_next          },
    { BTN_PREV_PIN,   bt_audio_prev          },
    { BTN_VOL_UP_PIN, bt_audio_vol_up        },
    { BTN_VOL_DN_PIN, bt_audio_vol_dn        },
    { BTN_PAIR_PIN,   bt_audio_enter_pairing },
};

static const int BTN_COUNT = sizeof(btn_map) / sizeof(btn_map[0]);

// --- ISR ---

static void IRAM_ATTR button_isr_handler(void * arg)
{
    btn_event_t evt  = { .pin = (int)(intptr_t)arg };
    BaseType_t woken = pdFALSE;
    xQueueSendFromISR(s_btn_queue, &evt, &woken);
    if (woken) portYIELD_FROM_ISR();
}

// --- task helpers ---

static bool is_button_still_pressed(int pin)
{
    return gpio_get_level((gpio_num_t)pin) == 0;
}

static void dispatch_button_action(int pin)
{
    for (int i = 0; i < BTN_COUNT; i++)
    {
        if (btn_map[i].pin != pin)
            continue;
        btn_map[i].action();
        return;
    }
    ESP_LOGW(TAG, "unhandled button pin %d", pin);
}

static void button_task(void * pv_params)
{
    btn_event_t evt;
    forever
    {
        if (xQueueReceive(s_btn_queue, &evt, portMAX_DELAY) != pdTRUE)
            continue;
        vTaskDelay(pdMS_TO_TICKS(BTN_DEBOUNCE_MS));
        if (!is_button_still_pressed(evt.pin))
            continue;
        dispatch_button_action(evt.pin);
    }
    vTaskDelete(NULL);
}

// --- GPIO setup ---

static void configure_button_gpio(int pin)
{
    gpio_config_t cfg = {
        .pin_bit_mask = (1ULL << pin),
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_NEGEDGE,
    };
    ESP_ERROR_CHECK(gpio_config(&cfg));
    ESP_ERROR_CHECK(gpio_isr_handler_add((gpio_num_t)pin, button_isr_handler,
                                         (void *)(intptr_t)pin));
}

// --- public API ---

void buttons_init(void)
{
    s_btn_queue = xQueueCreate(10, sizeof(btn_event_t));
    ESP_ERROR_CHECK(gpio_install_isr_service(0));

    for (int i = 0; i < BTN_COUNT; i++)
        configure_button_gpio(btn_map[i].pin);

    xTaskCreate(button_task, "button_task", 2048, nullptr, 5, nullptr);
    ESP_LOGI(TAG, "initialized %d buttons", BTN_COUNT);
}
