#ifndef AUDIO_NOTIFY_H
#define AUDIO_NOTIFY_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
#include "AudioTools.h"
// set_stream must be called once before any audio_notify_play calls
void audio_notify_set_stream(I2SStream & stream);

extern "C" {
#endif

void audio_notify_play(const uint8_t * data, size_t len);

#ifdef __cplusplus
}
#endif

#endif // AUDIO_NOTIFY_H
