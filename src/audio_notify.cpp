#include <stddef.h>

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "AudioTools.h"
#include "AudioTools/AudioCodecs/CodecMP3Helix.h"

#include "app_config.h"
#include "audio_notify.h"
#include "bt_audio.h"

static const char * TAG = "audio_notify";

static AudioOutput_t *  s_out_stream   = nullptr;
static QueueHandle_t    s_clip_queue   = nullptr;

typedef struct { const uint8_t * data; size_t len; } clip_t;

void audio_notify_set_stream(AudioOutput_t & stream)
{
    s_out_stream = &stream;
}

static void decode_and_play_mp3(const uint8_t * data, size_t len)
{
    MemoryStream mp3_mem(data, len);
    MP3DecoderHelix mp3_dec;
    EncodedAudioStream decoder(s_out_stream, &mp3_dec);
    StreamCopy copier(decoder, mp3_mem);

    decoder.begin();
    while (mp3_mem.available())
        copier.copy();
    decoder.end();
}

static void audio_notify_task(void * pv)
{
    clip_t clip;
    forever
    {
        if (xQueueReceive(s_clip_queue, &clip, portMAX_DELAY) != pdTRUE)
            continue;
        bt_audio_set_output_active(false);
        decode_and_play_mp3(clip.data, clip.len);
        bt_audio_set_output_active(true);
    }
    vTaskDelete(NULL);
}

void audio_notify_init(void)
{
    s_clip_queue = xQueueCreate(2, sizeof(clip_t));
    xTaskCreate(audio_notify_task, "audio_notify", 8192, nullptr, 4, nullptr);
    ESP_LOGI(TAG, "initialized");
}

void audio_notify_play(const uint8_t * data, size_t len)
{
#if DEBUG_NO_CONTROL_AUDIO
    return;
#endif
    if (len == 0 || s_out_stream == nullptr || s_clip_queue == nullptr)
        return;
    clip_t clip = { data, len };
    xQueueSend(s_clip_queue, &clip, 0);
}
