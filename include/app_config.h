#ifndef CONFIG_H
#define CONFIG_H

// --- Bluetooth ---
#define BT_DEVICE_NAME      "Panasonic RX-M70M2"
#define BT_AUTO_RECONNECT   true
#define BT_RECONNECT_COUNT  5

// --- I2S DAC pins ---
#define I2S_BCLK_PIN        14
#define I2S_WCLK_PIN        15
#define I2S_DATA_PIN        22

// --- Button GPIOs ---
#define BTN_PAUSE_PIN       32
#define BTN_NEXT_PIN        33
#define BTN_PREV_PIN        19
#define BTN_VOL_UP_PIN      21
#define BTN_VOL_DN_PIN      12
#define BTN_PAIR_PIN        13

// --- Debounce ---
#define BTN_DEBOUNCE_MS     50

// --- Onboard LED ---
#define ONBOARD_LED_PIN     2

// --- Utility ---
#ifndef forever
#define forever for(;;)
#endif

#endif // CONFIG_H
