#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include <stdbool.h>

void keyboard_init(void);

// Returns true while the autoclicker is toggled on
bool autoclicker_on(bool button_pressed);

#endif
