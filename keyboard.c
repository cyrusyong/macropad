#include <stdlib.h>
#include <stdint.h>
#include "bsp/board.h"
#include "tusb.h"
#include "keyboard.h"

// --- Switch 1: F-key autoclicker ---
static bool     sw1_key_down    = false;
static uint32_t sw1_key_down_at = 0;
static uint32_t sw1_next_send   = 0;

static void sw1_task(bool active) {
    uint32_t now = board_millis();

    if (active) {
        if (sw1_key_down) {
            if (now - sw1_key_down_at >= 50) {
                tud_hid_keyboard_report(0, 0, NULL);
                sw1_key_down  = false;
                sw1_next_send = now + 500 + rand() % 1001;
            }
        } else if (now >= sw1_next_send) {
            uint8_t keys[6] = {HID_KEY_F};
            tud_hid_keyboard_report(0, 0, keys);
            sw1_key_down    = true;
            sw1_key_down_at = now;
        }
    } else if (sw1_key_down) {
        tud_hid_keyboard_report(0, 0, NULL);
        sw1_key_down  = false;
        sw1_next_send = 0;
    }
}

// --- Switch 2: TBD ---
static void sw2_task(bool active) {
    (void)active;
}

// --- Switch 3: TBD ---
static void sw3_task(bool active) {
    (void)active;
}

// --- Switch 4: TBD ---
static void sw4_task(bool active) {
    (void)active;
}

void keyboard_init(void) {
    srand(time_us_32());
}

void keyboard_task(bool sw[4]) {
    sw1_task(sw[0]);
    sw2_task(sw[1]);
    sw3_task(sw[2]);
    sw4_task(sw[3]);
}
