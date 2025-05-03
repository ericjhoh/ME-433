/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/adc.h"

#define FLAG_VALUE 123
#define FLAG_READ_VOLTAGE 0
#define FLAG_LED_ON 1
#define FLAG_LED_OFF 2


static int voltage = 0;
static int led = 0;

void core1_entry() {
    gpio_init(15);
    gpio_set_dir(15, GPIO_OUT);
    adc_init();
    adc_gpio_init(26); // A0 is GPIO 26
    adc_select_input(0);

    while(1) {
        uint32_t g = multicore_fifo_pop_blocking();

        if(g == FLAG_READ_VOLTAGE) {
            voltage = adc_read();
            multicore_fifo_push_blocking(voltage);

        } 
        else if(g == FLAG_LED_ON) {
            gpio_put(15, 1);
            led = 1;
            multicore_fifo_push_blocking(led);


        }
        else if(g == FLAG_LED_OFF) {
            gpio_put(15, 0);
            led = 0;
            multicore_fifo_push_blocking(led);
        }
    }

}

int main() {
    stdio_init_all();

    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    printf("Hello, multicore!\n");


    /// \tag::setup_multicore[]

    multicore_launch_core1(core1_entry);

    // Wait for it to start up

    while(1) {
        int send = 0;
        printf("Enter 0, 1, or 2: ");
        scanf("%d", &send);
        printf("You entered %d\n", send);
        if(send == 0) {
            multicore_fifo_push_blocking(FLAG_READ_VOLTAGE);

        }
        else if(send == 1) {
            multicore_fifo_push_blocking(FLAG_LED_ON);

        }

        else if(send == 2) {
            multicore_fifo_push_blocking(FLAG_LED_OFF);
        }


        uint32_t g = multicore_fifo_pop_blocking();

        printf("Value back = %d\n", g);



    }


    /// \end::setup_multicore[]
}
