#ifndef SOUNDS_H
#define SOUNDS_H

#include <stddef.h>
#include <stdint.h>

// Fill these arrays with MP3-encoded bytes, e.g.:
//   xxd -i connect.mp3 | head  →  copy the array body here

static const uint8_t connect_sound_data[]    = {};
static const size_t  connect_sound_len       = 0;

static const uint8_t disconnect_sound_data[] = {};
static const size_t  disconnect_sound_len    = 0;

static const uint8_t pairing_sound_data[]    = {};
static const size_t  pairing_sound_len       = 0;

#endif // SOUNDS_H
