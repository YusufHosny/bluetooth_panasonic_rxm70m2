#include <stdbool.h>

#include "esp_log.h"
#include "AudioTools.h"
#include "BluetoothA2DPSink.h"

#include "app_config.h"
#include "bt_audio.h"
#include "audio_notify.h"
#include "sounds.h"

static const char * TAG = "bt_audio";

#if USE_I2S_OUT
static I2SStream s_out_stream;
static BluetoothA2DPSink s_a2dp_sink(s_out_stream);
#elif USE_INTERNAL_DAC_OUT
static AnalogAudioStream s_dac_out;

// Sits between A2DP and the DAC. Muting drops A2DP writes without touching
// the DAC driver, so audio_notify can write to s_dac_out concurrently.
// begin()/end() are no-ops so A2DP's set_output_active can't tear down the DAC.
class GatedStream : public AudioStream {
public:
    GatedStream(AudioStream & out) : _out(out) {}
    size_t write(const uint8_t * data, size_t len) override {
        if (_muted) return len;
        return _out.write(data, len);
    }
    bool   begin()   override { return true; }
    void   end()     override {}
    void   setAudioInfo(AudioInfo info) override { _out.setAudioInfo(info); }
    AudioInfo audioInfo()      override { return _out.audioInfo(); }
    int    available()         override { return 0; }
    int    read()              override { return -1; }
    int    peek()              override { return -1; }
    void   set_muted(bool m)            { _muted = m; }
private:
    AudioStream & _out;
    volatile bool _muted = false;
};

static GatedStream        s_out_stream(s_dac_out);
static BluetoothA2DPSink  s_a2dp_sink(s_out_stream);
#endif
static bool s_is_playing = false;

// --- output setup ---
static void start_output(void)
{
#if USE_I2S_OUT
    auto cfg = s_out_stream.defaultConfig(TX_MODE);
    cfg.pin_bck  = I2S_BCLK_PIN;
    cfg.pin_ws   = I2S_WCLK_PIN;
    cfg.pin_data = I2S_DATA_PIN;
    s_out_stream.begin(cfg);
#elif USE_INTERNAL_DAC_OUT
    auto cfg = s_dac_out.defaultConfig();
    cfg.sample_rate = 44100;
    cfg.channels    = 2;
    s_dac_out.begin(cfg);
#endif
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
    start_output();
#if USE_INTERNAL_DAC_OUT
    audio_notify_set_stream(s_dac_out);
#else
    audio_notify_set_stream(s_out_stream);
#endif
    audio_notify_init();
    register_callbacks();
    s_a2dp_sink.set_mono_downmix(true);
    s_a2dp_sink.set_auto_reconnect(BT_AUTO_RECONNECT, BT_RECONNECT_COUNT);
    s_a2dp_sink.start(BT_DEVICE_NAME);

#if DEBUG_PLAY_PAIR_AUDIO_ON_STARTUP
    delay(5000);
    audio_notify_play(disconnect_sound_data, disconnect_sound_len);
#endif

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
#if USE_INTERNAL_DAC_OUT
    s_out_stream.set_muted(!active);
#endif
}
