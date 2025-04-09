/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define GPIO_WATCH_PIN 2

static int num_press = 0;
static int state = 0;

void gpio_callback(uint gpio, uint32_t events) {
    // Put the GPIO event(s) that just happened into event_str
    // so we can print it
    num_press++;
    if(state == 0 ) {
        state = 1;
    } else {
        state = 0;
    }

    gpio_put(3, state);
    printf("Button Press #%d\n", num_press);
}

int main() {
    stdio_init_all();

    while(!stdio_usb_connected()){
        sleep_ms(100);
    }

    printf("Start\n");
    gpio_init(GPIO_WATCH_PIN);
    gpio_init(3);
    gpio_set_dir(3, GPIO_OUT);
    gpio_put(3, state);
    gpio_set_irq_enabled_with_callback(GPIO_WATCH_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    // Wait forever
    while (1);
}


static const char *gpio_irq_str[] = {
        "LEVEL_LOW",  // 0x1
        "LEVEL_HIGH", // 0x2
        "EDGE_FALL",  // 0x4
        "EDGE_RISE"   // 0x8
};
