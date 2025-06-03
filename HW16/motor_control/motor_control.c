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

// A is left motor
#define PWMA 17
#define DIRA 16

// B is right motor
#define PWMB 19
#define DIRB 18

#define WRAP 50000



void init_PWM_DIR() {
    gpio_set_function(PWMA, GPIO_FUNC_PWM); // Set the LED Pin to be PWM
    gpio_set_function(PWMB, GPIO_FUNC_PWM); // Set the LED Pin to be PWM

    gpio_init(DIRA);
    gpio_init(DIRB);
    
    gpio_set_dir(DIRA, GPIO_OUT);
    gpio_set_dir(DIRB, GPIO_OUT);

    // moving forward
    gpio_put(DIRA, 1); //ccw when facing wheel
    gpio_put(DIRB, 0); //cw when facing wheel

    float div = 60; // must be between 1-255

    uint slice_numA = pwm_gpio_to_slice_num(PWMA); // Get PWM slice number
    pwm_set_clkdiv(slice_numA, div); // divider
    pwm_set_wrap(slice_numA, WRAP);
    pwm_set_enabled(slice_numA, true); // turn on the PWM

    uint slice_numB = pwm_gpio_to_slice_num(PWMB); // Get PWM slice number
    pwm_set_clkdiv(slice_numB, div); // divider
    pwm_set_wrap(slice_numB, WRAP);
    pwm_set_enabled(slice_numB, true); // turn on the PWM


}


int main() {
    stdio_init_all();

    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }

    init_PWM_DIR();

   
    while(1) {
        pwm_set_gpio_level(PWMA, WRAP / 2); // set the duty cycle to 50%
        pwm_set_gpio_level(PWMB, WRAP / 2); // set the duty cycle to 50%
        sleep_ms(3000);
        pwm_set_gpio_level(PWMA, WRAP); // set the duty cycle to 50%
        pwm_set_gpio_level(PWMB, WRAP); // set the duty cycle to 50%
        sleep_ms(3000);
    }
}
