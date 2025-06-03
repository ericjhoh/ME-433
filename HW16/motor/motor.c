/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

// This example drives a PWM output at a range of duty cycles, and uses
// another PWM slice in input mode to measure the duty cycle. You'll need to
// connect these two pins with a jumper wire:


#define PWMA 17
#define DIRA 16

#define PWMB 19
#define DIRB 18

void init_PWM_DIR() {
    gpio_set_function(PWMA, GPIO_FUNC_PWM); // Set the LED Pin to be PWM
    gpio_set_function(PWMB, GPIO_FUNC_PWM); // Set the LED Pin to be PWM

    gpio_init(DIRA);
    gpio_init(DIRB);
    
    gpio_set_dir(DIRA, GPIO_OUT);
    gpio_set_dir(DIRB, GPIO_OUT);

    gpio_put(DIRA, 0);
    gpio_put(DIRB, 0);

    uint slice_num = pwm_gpio_to_slice_num(PWMA); // Get PWM slice number
    float div = 60; // must be between 1-255
    pwm_set_clkdiv(slice_num, div); // divider
    uint16_t wrap = 50000; // when to rollover, must be less than 65535
    pwm_set_wrap(slice_num, wrap);
    pwm_set_enabled(slice_num, true); // turn on the PWM

    pwm_set_gpio_level(PWMA, wrap / 2); // set the duty cycle to 50%
    pwm_set_gpio_level(PWMB, wrap / 2); // set the duty cycle to 50%

}


int main() {
    stdio_init_all();
    init_PWM_DIR();

    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }

    while(1) {
        
    }
}
