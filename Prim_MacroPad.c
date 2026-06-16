#include <stdio.h>
#include "pico/stdlib.h"

#define SW_1 28
#define LED_GREEN 13


int main()
{
    gpio_init(SW_1);
    gpio_set_dir(SW_1, GPIO_IN);
    gpio_pull_down(SW_1);

    gpio_init(LED_GREEN);
    gpio_set_dir(LED_GREEN, GPIO_OUT);

    while (true) {
        bool switch_on = gpio_get(SW_1);

        if (switch_on) {
            gpio_put(LED_GREEN, 1);
        } else {
            gpio_put(LED_GREEN, 0);
        }
    }
}
