#include "pico/stdlib.h"
#include "bsp/board.h"
#include "tusb.h"
#include "keyboard.h"

#define SW_1  28
#define SW_2  27
#define SW_3  26
#define SW_4  22

#define LED_1 13

static void init_switch(uint gpio)
{
    gpio_init(gpio);
    gpio_set_dir(gpio, GPIO_IN);
    gpio_pull_down(gpio);
}

int main()
{
    init_switch(SW_1);
    init_switch(SW_2);
    init_switch(SW_3);
    init_switch(SW_4);

    gpio_init(LED_1);
    gpio_set_dir(LED_1, GPIO_OUT);

    tusb_init();
    keyboard_init();

    while (true)
    {
        tud_task();

        // SW_1: Autoclicker
        bool led_on = autoclicker_on(gpio_get(SW_1));
        gpio_put(LED_1, led_on);

        // SW_2: Open application by typing in Start Menu
        open_application(gpio_get(SW_2), "genshin impact");

        // SW_3: Open Discord, search for user, send them something
        send_disc_message(gpio_get(SW_3), "prim", "i love you!!");

        // SW_4: Shutdown PC after 30 seconds, abort if needed
        shutdown_task(gpio_get(SW_4));
    }
}
