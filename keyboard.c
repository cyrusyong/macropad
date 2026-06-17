#include <stdlib.h>
#include <stdint.h>
#include "bsp/board.h"
#include "tusb.h"
#include "keyboard.h"

static void hid_wait(uint32_t ms);
static void type_key(uint8_t key, uint8_t modifier, uint32_t ms_wait);
static uint8_t ascii_to_hid(char c);

// ----------------------------------------------------------------
// Switch 1 — F-key autoclicker (toggle)
// ----------------------------------------------------------------
static bool     autoclicker_active  = false;
static bool     prev_button         = false;
static uint32_t stable_since        = 0;
static bool     toggled_this_press  = false;
static bool     key_down            = false;
static uint32_t key_down_at         = 0;
static uint32_t next_send           = 0;

// Button must be held high continuously for this long before toggling
#define DEBOUNCE_MS 50

bool autoclicker_on(bool button_pressed)
{
    uint32_t now = board_millis();

    if (button_pressed) {
        if (!prev_button) {
            // Button just went high — start the stability timer
            stable_since = now;
            toggled_this_press = false;
        } else if (!toggled_this_press && (now - stable_since >= DEBOUNCE_MS)) {
            // Button has been stable high for long enough — toggle once
            autoclicker_active = !autoclicker_active;
            toggled_this_press = true;

            if (!autoclicker_active && key_down) {
                tud_hid_keyboard_report(0, 0, NULL);
                key_down  = false;
                next_send = 0;
            }
        }
    }
    prev_button = button_pressed;

    if (autoclicker_active)
    {
        if (key_down)
        {
            if (now - key_down_at >= 50)
            {
                tud_hid_keyboard_report(0, 0, NULL);
                key_down = false;
                next_send = now + 500 + rand() % 1001;
            }
        }
        else if (now >= next_send)
        {
            uint8_t keys[6] = {HID_KEY_F};
            tud_hid_keyboard_report(0, 0, keys);
            key_down = true;
            key_down_at = now;
        }
    }

    return autoclicker_active;
}

// ----------------------------------------------------------------
// Switch 2 — Open Applicaiton
// ----------------------------------------------------------------
void open_application(const char *application_name)
{
    // Windows Key Press
    type_key(0, KEYBOARD_MODIFIER_LEFTGUI, 500);

    // Type application name
    for (int i = 0; application_name[i]; i++) {
        type_key(ascii_to_hid(application_name[i]), 0, 10);
    }

    // Hit enter
    type_key(HID_KEY_ENTER, 0, 50);
}

// ----------------------------------------------------------------
// Switch 3 — TBD
// ----------------------------------------------------------------

// ----------------------------------------------------------------
// Switch 4 — TBD
// ----------------------------------------------------------------

void keyboard_init(void)
{
    srand(time_us_32());
}

static void hid_wait(uint32_t ms)
{
    absolute_time_t deadline = make_timeout_time_ms(ms);
    while (!time_reached(deadline))
    {
        tud_task();
    }
}

static void type_key(uint8_t key, uint8_t modifier, uint32_t ms_wait)
{
    uint8_t keys[6] = {key};
    tud_hid_keyboard_report(0, modifier, keys);
    hid_wait(ms_wait);

    tud_hid_keyboard_report(0, 0, NULL);
    hid_wait(ms_wait);
}

static uint8_t ascii_to_hid(char c) {
    if (c >= 'a' && c <= 'z') return HID_KEY_A + (c - 'a');
    if (c >= 'A' && c <= 'Z') return HID_KEY_A + (c - 'A');
    if (c >= '1' && c <= '9') return HID_KEY_1 + (c - '1');
    if (c == '0')              return HID_KEY_0;
    if (c == ' ')              return HID_KEY_SPACE;
    if (c == '\n')             return HID_KEY_ENTER;
    if (c == '\t')             return HID_KEY_TAB;
    return 0;
}

