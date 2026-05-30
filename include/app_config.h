#ifndef CONFIG_H
#define CONFIG_H

// --- Config ---
#define USE_I2S_OUT 0
#define USE_INTERNAL_DAC_OUT !USE_I2S_OUT

#define DEBUG_NO_CONTROL_AUDIO 0


// --- Bluetooth ---
#define BT_DEVICE_NAME      "Panasonic RX-M70M2"
#define BT_AUTO_RECONNECT   true
#define BT_RECONNECT_COUNT  5

// --- I2S DAC pins ---
#define I2S_BCLK_PIN        5
#define I2S_WCLK_PIN        18
#define I2S_DATA_PIN        19

// --- Button GPIOs ---
#define BTN_PAUSE_PIN       2
#define BTN_NEXT_PIN        23
#define BTN_PREV_PIN        19
#define BTN_VOL_UP_PIN      25
#define BTN_VOL_DN_PIN      27
#define BTN_PAIR_PIN        12

// --- Debounce ---
#define BTN_DEBOUNCE_MS     50

// --- Utility ---
#ifndef forever
#define forever for(;;)
#endif

#endif // CONFIG_H
