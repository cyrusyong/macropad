#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include <stdbool.h>

void keyboard_init(void);

bool autoclicker_on(bool button_pressed);
void open_application(bool button_pressed, const char *application_name);
void send_disc_message(bool button_pressed, const char *username, const char *message);
void shutdown_task(bool button_pressed);


#endif
