#include <stdlib.h>
#include <stdint.h>
#include "bsp/board.h"
#include "tusb.h"
#include "keyboard.h"

typedef struct {
    uint8_t key;
    uint8_t modifier;
} hid_key_t;

static void      hid_wait(uint32_t ms);
static void      type_key(uint8_t key, uint8_t modifier, uint32_t ms_wait);
static void      type_string(const char *str);
static void      run_command(const char *cmd);
static hid_key_t ascii_to_hid(char c);

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

#define DEBOUNCE_MS 50

bool autoclicker_on(bool button_pressed)
{
    uint32_t now = board_millis();

    if (button_pressed) {
        if (!prev_button) {
            stable_since       = now;
            toggled_this_press = false;
        } else if (!toggled_this_press && (now - stable_since >= DEBOUNCE_MS)) {
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

    if (autoclicker_active) {
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
// Switch 2 — Open Application
// ----------------------------------------------------------------
static bool sw2_prev = false;

void open_application(bool button_pressed, const char *application_name)
{
    if (button_pressed && !sw2_prev) {
        type_key(0, KEYBOARD_MODIFIER_LEFTGUI, 50);
        type_string(application_name);
        type_key(HID_KEY_ENTER, 0, 50);
    }
    sw2_prev = button_pressed;
}

// ----------------------------------------------------------------
// Switch 3 — Send message on Discord
// ----------------------------------------------------------------
static bool sw3_prev = false;

void send_disc_message(bool button_pressed, const char *username, const char *message)
{
    if (button_pressed && !sw3_prev) {
        // Focus Discord via Start Menu, then wait for it to open
        type_key(0, KEYBOARD_MODIFIER_LEFTGUI, 50);
        type_string("discord");
        type_key(HID_KEY_ENTER, 0, 50);
        hid_wait(500);

        // Search for user with Ctrl+K and send message
        type_key(HID_KEY_K, KEYBOARD_MODIFIER_LEFTCTRL, 50);
        type_string(username);
        type_key(HID_KEY_ENTER, 0, 50);

        type_string(message);
        type_key(HID_KEY_ENTER, 0, 50);
    }
    sw3_prev = button_pressed;
}

// ----------------------------------------------------------------
// Switch 4 — Shutdown PC
// ----------------------------------------------------------------
typedef enum {
    SHUTDOWN_IDLE,
    SHUTDOWN_PENDING,
} shutdown_state_t;

static shutdown_state_t shutdown_state = SHUTDOWN_IDLE;
static bool             sw4_prev       = false;
static uint32_t         sw4_held_since = 0;
static bool             sw4_triggered  = false;

void shutdown_task(bool button_pressed)
{
    uint32_t now = board_millis();

    if (shutdown_state == SHUTDOWN_IDLE) {
        if (button_pressed) {
            if (!sw4_prev) {
                sw4_held_since = now;
                sw4_triggered  = false;
            } else if (!sw4_triggered && (now - sw4_held_since >= 3000)) {
                run_command("shutdown /s /t 30");
                shutdown_state = SHUTDOWN_PENDING;
                sw4_triggered  = true;
            }
        }
    } else {
        // Shutdown pending — any press cancels it
        if (button_pressed && !sw4_prev) {
            run_command("shutdown /a");
            shutdown_state = SHUTDOWN_IDLE;
        }
    }

    sw4_prev = button_pressed;
}

// ================================================================
// Helpers
// ================================================================

void keyboard_init(void)
{
    srand(time_us_32());
}

static void hid_wait(uint32_t ms)
{
    absolute_time_t deadline = make_timeout_time_ms(ms);
    while (!time_reached(deadline)) {
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

static void type_string(const char *str)
{
    for (int i = 0; str[i]; i++) {
        hid_key_t k = ascii_to_hid(str[i]);
        type_key(k.key, k.modifier, 10);
    }
}

static void run_command(const char *cmd)
{
    type_key(HID_KEY_R, KEYBOARD_MODIFIER_LEFTGUI, 500);
    type_string(cmd);
    type_key(HID_KEY_ENTER, 0, 50);
}

static hid_key_t ascii_to_hid(char c)
{
    #define K(k)  ((hid_key_t){k, 0})
    #define KS(k) ((hid_key_t){k, KEYBOARD_MODIFIER_LEFTSHIFT})

    if (c >= 'a' && c <= 'z') return K(HID_KEY_A + (c - 'a'));
    if (c >= 'A' && c <= 'Z') return KS(HID_KEY_A + (c - 'A'));
    if (c >= '1' && c <= '9') return K(HID_KEY_1 + (c - '1'));
    if (c == '0')  return K(HID_KEY_0);
    if (c == ' ')  return K(HID_KEY_SPACE);
    if (c == '\n') return K(HID_KEY_ENTER);
    if (c == '\t') return K(HID_KEY_TAB);
    if (c == '/')  return K(HID_KEY_SLASH);
    if (c == '!')  return KS(HID_KEY_1);
    if (c == '@')  return KS(HID_KEY_2);
    if (c == '#')  return KS(HID_KEY_3);
    if (c == '$')  return KS(HID_KEY_4);
    if (c == '%')  return KS(HID_KEY_5);
    if (c == '^')  return KS(HID_KEY_6);
    if (c == '&')  return KS(HID_KEY_7);
    if (c == '*')  return KS(HID_KEY_8);
    if (c == '(')  return KS(HID_KEY_9);
    if (c == ')')  return KS(HID_KEY_0);

    #undef K
    #undef KS
    return (hid_key_t){0, 0};
}