#ifndef AUDIO_NOTIFY_H
#define AUDIO_NOTIFY_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
#include "AudioTools.h"
#include "app_config.h"

#if USE_I2S_OUT
typedef I2SStream AudioOutput_t;
#elif USE_INTERNAL_DAC_OUT
typedef AnalogAudioStream AudioOutput_t;
#endif

void audio_notify_set_stream(AudioOutput_t & stream);
void audio_notify_init(void);

extern "C" {
#endif

void audio_notify_play(const uint8_t * data, size_t len);

#ifdef __cplusplus
}
#endif

#endif // AUDIO_NOTIFY_H
