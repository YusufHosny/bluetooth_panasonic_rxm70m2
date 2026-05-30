#ifndef BT_AUDIO_H
#define BT_AUDIO_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void bt_audio_init(void);

// controls sent to master device
void bt_audio_pause(void);
void bt_audio_resume(void);
void bt_audio_toggle_pause(void);
void bt_audio_next(void);
void bt_audio_prev(void);
void bt_audio_vol_up(void);
void bt_audio_vol_dn(void);

// Clears NVS, drops connection, plays pairing sound, becomes discoverable
void bt_audio_enter_pairing(void);

bool bt_audio_is_connected(void);
bool bt_audio_is_playing(void);

// Gates A2DP writes to the DAC without closing the stream
void bt_audio_set_output_active(bool active);


#ifdef __cplusplus
}
#endif

#endif // BT_AUDIO_H
