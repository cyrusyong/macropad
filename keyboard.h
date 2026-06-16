#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include <stdbool.h>

void keyboard_init(void);
void keyboard_task(bool sw[4]);

#endif
