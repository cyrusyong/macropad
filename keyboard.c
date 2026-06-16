#include <stdlib.h>
#include <stdint.h>
#include "bsp/board.h"
#include "tusb.h"
#include "keyboard.h"

// ----------------------------------------------------------------
// Switch 1 — F-key autoclicker (toggle)
// ----------------------------------------------------------------
static bool     autoclicker_active  = false;
static bool     prev_button         = false;
static bool     key_down            = false;
static uint32_t key_down_at         = 0;
static uint32_t next_send           = 0;

bool autoclicker_on(bool button_pressed) {
    // Toggle on rising edge (button just pressed)
    if (button_pressed && !prev_button) {
        autoclicker_active = !autoclicker_active;

        if (!autoclicker_active && key_down) {
            tud_hid_keyboard_report(0, 0, NULL);
            key_down  = false;
            next_send = 0;
        }
    }
    prev_button = button_pressed;

    if (autoclicker_active) {
        uint32_t now = board_millis();

        if (key_down) {
            if (now - key_down_at >= 50) {
                tud_hid_keyboard_report(0, 0, NULL);
                key_down  = false;
                next_send = now + 500 + rand() % 1001;
            }
        } else if (now >= next_send) {
            uint8_t keys[6] = {HID_KEY_F};
            tud_hid_keyboard_report(0, 0, keys);
            key_down    = true;
            key_down_at = now;
        }
    }

    return autoclicker_active;
}

// ----------------------------------------------------------------
// Switch 2 — TBD
// ----------------------------------------------------------------

// ----------------------------------------------------------------
// Switch 3 — TBD
// ----------------------------------------------------------------

// ----------------------------------------------------------------
// Switch 4 — TBD
// ----------------------------------------------------------------

void keyboard_init(void) {
    srand(time_us_32());
}
