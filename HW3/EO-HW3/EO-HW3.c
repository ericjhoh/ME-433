#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"


#define BUTTON 15
#define LED 16



int main() {
    stdio_init_all();

    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }

    gpio_init(BUTTON);
    gpio_init(LED);

    // set input outputs
    gpio_set_dir(BUTTON, GPIO_IN);
    gpio_set_dir(LED, GPIO_OUT);

    // turn LED on
    gpio_put(LED, 1);

    while(!gpio_get(BUTTON)) {}

    gpio_put(LED, 0);

    adc_init(); // init the adc module
    adc_gpio_init(26); // set ADC0 pin to be adc input instead of GPIO
    adc_select_input(0); // select to read from ADC0

    while(1) {
        int analog_samples = 0;
        int i = 0;
        printf("Analog Samples (1-100): ");
        scanf("%d", &analog_samples);

        for(i = 0; i < analog_samples; i++) {
            
            // reads voltage
            uint16_t result = adc_read();

            // converts raw nuumber into voltage
            float voltage = 3.3 * result / 4095.0;
            printf("%d: %f V\r\n", i + 1, voltage);

            sleep_ms(10);
        }

    }
}

