#include <stdbool.h>

#include "esp_log.h"
#include "AudioTools.h"
#include "BluetoothA2DPSink.h"

#include "config.h"
#include "bt_audio.h"
#include "audio_notify.h"
#include "sounds.h"

static const char * TAG = "bt_audio";

static I2SStream s_i2s_stream;
static BluetoothA2DPSink s_a2dp_sink(s_i2s_stream);
static bool s_is_playing = false;

// --- I2S setup ---

static void start_i2s_output(void)
{
    auto cfg = s_i2s_stream.defaultConfig(TX_MODE);
    cfg.pin_bck  = I2S_BCLK_PIN;
    cfg.pin_ws   = I2S_WCLK_PIN;
    cfg.pin_data = I2S_DATA_PIN;
    s_i2s_stream.begin(cfg);
}

// --- A2DP callbacks ---

static void on_device_connected(void)
{
    ESP_LOGI(TAG, "device connected");
    audio_notify_play(connect_sound_data, connect_sound_len);
}

static void on_device_disconnected(void)
{
    ESP_LOGI(TAG, "device disconnected");
    s_is_playing = false;
    audio_notify_play(disconnect_sound_data, disconnect_sound_len);
}

static void on_connection_state_changed(esp_a2d_connection_state_t state, void * obj)
{
    if (state == ESP_A2D_CONNECTION_STATE_CONNECTED)
        on_device_connected();
    else if (state == ESP_A2D_CONNECTION_STATE_DISCONNECTED)
        on_device_disconnected();
}

static void on_playback_status_changed(esp_avrc_playback_stat_t status)
{
    s_is_playing = (status == ESP_AVRC_PLAYBACK_PLAYING);
    ESP_LOGI(TAG, "playback status: %d", (int)status);
}

static void register_callbacks(void)
{
    s_a2dp_sink.set_on_connection_state_changed(on_connection_state_changed, nullptr);
    s_a2dp_sink.set_avrc_rn_playstatus_callback(on_playback_status_changed);
}

// --- public API ---

void bt_audio_init(void)
{
    start_i2s_output();
    audio_notify_set_stream(s_i2s_stream);
    register_callbacks();
    s_a2dp_sink.set_auto_reconnect(BT_AUTO_RECONNECT, BT_RECONNECT_COUNT);
    s_a2dp_sink.start(BT_DEVICE_NAME);
    ESP_LOGI(TAG, "started as '%s'", BT_DEVICE_NAME);
}

void bt_audio_pause(void)
{
    s_a2dp_sink.pause();
}

void bt_audio_resume(void)
{
    s_a2dp_sink.play();
}

void bt_audio_toggle_pause(void)
{
    if (s_is_playing) s_a2dp_sink.pause();
    else              s_a2dp_sink.play();
}

void bt_audio_next(void)
{
    s_a2dp_sink.next();
}

void bt_audio_prev(void)
{
    s_a2dp_sink.previous();
}

void bt_audio_vol_up(void)
{
    s_a2dp_sink.volume_up();
}

void bt_audio_vol_dn(void)
{
    s_a2dp_sink.volume_down();
}

void bt_audio_enter_pairing(void)
{
    s_a2dp_sink.disconnect();
    s_a2dp_sink.clean_last_connection();
    audio_notify_play(pairing_sound_data, pairing_sound_len);
    ESP_LOGI(TAG, "pairing mode: NVS cleared, now discoverable");
}

bool bt_audio_is_connected(void)
{
    return s_a2dp_sink.is_connected();
}

bool bt_audio_is_playing(void)
{
    return s_is_playing;
}

void bt_audio_set_output_active(bool active)
{
    if (s_a2dp_sink.get_output() == nullptr)
        return;
    s_a2dp_sink.get_output()->set_output_active(active);
}
