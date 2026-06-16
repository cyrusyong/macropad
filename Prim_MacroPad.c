#include "pico/stdlib.h"
#include "bsp/board.h"
#include "tusb.h"
#include "keyboard.h"

#define SW_1  28
#define SW_2  27
#define SW_3  26
#define SW_4  22

#define LED_1 13

int main()
{
    gpio_init(SW_1);
    gpio_set_dir(SW_1, GPIO_IN);
    gpio_pull_down(SW_1);

    gpio_init(SW_2);
    gpio_set_dir(SW_2, GPIO_IN);
    gpio_pull_down(SW_2);

    gpio_init(SW_3);
    gpio_set_dir(SW_3, GPIO_IN);
    gpio_pull_down(SW_3);

    gpio_init(SW_4);
    gpio_set_dir(SW_4, GPIO_IN);
    gpio_pull_down(SW_4);

    gpio_init(LED_1);
    gpio_set_dir(LED_1, GPIO_OUT);

    tusb_init();
    keyboard_init();

    while (true)
    {
        tud_task();

        bool sw[4] = {
            gpio_get(SW_1),
            gpio_get(SW_2),
            gpio_get(SW_3),
            gpio_get(SW_4),
        };

        gpio_put(LED_1, sw[0]);

        keyboard_task(sw);
    }
}
