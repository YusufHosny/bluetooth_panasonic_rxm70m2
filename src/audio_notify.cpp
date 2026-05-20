#include <stddef.h>

#include "esp_log.h"
#include "AudioTools.h"
#include "AudioTools/AudioCodecs/CodecMP3Helix.h"

#include "audio_notify.h"
#include "bt_audio.h"

static const char * TAG = "audio_notify";

static I2SStream * s_i2s_stream = nullptr;

void audio_notify_set_stream(I2SStream & stream)
{
    s_i2s_stream = &stream;
}

static void decode_and_play_mp3(const uint8_t * data, size_t len)
{
    MemoryStream mp3_mem(data, len);
    MP3DecoderHelix mp3_dec;
    EncodedAudioStream decoder(s_i2s_stream, &mp3_dec);
    StreamCopy copier(decoder, mp3_mem);

    decoder.begin();
    while (mp3_mem.available())
    {
        copier.copy();
    }
    decoder.end();
}

void audio_notify_play(const uint8_t * data, size_t len)
{
    if (len == 0 || s_i2s_stream == nullptr)
        return;

    bt_audio_set_output_active(false);
    decode_and_play_mp3(data, len);
    bt_audio_set_output_active(true);
}
