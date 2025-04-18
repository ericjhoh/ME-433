#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"

// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_CS   17
#define PIN_SCK  18
#define PIN_MOSI 19`



int main()
{
    stdio_init_all();

    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }

    // // SPI initialisation. This example will use SPI at 1MHz.
    // spi_init(SPI_PORT, 1000*1000);
    // gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    // gpio_set_function(PIN_CS,   GPIO_FUNC_SIO);
    // gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    // gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    
    // // Chip select is active-low, so we'll initialise it to a driven-high state
    // gpio_set_dir(PIN_CS, GPIO_OUT);
    // gpio_put(PIN_CS, 1);
    // // For more examples of SPI use see https://github.com/raspberrypi/pico-examples/tree/master/spi

    volatile float f1, f2;
    while(1) {
        printf("Enter two floats to use:");
        scanf("%f %f", &f1, &f2);
        volatile float f_add, f_sub, f_mult, f_div;

        absolute_time_t t = get_absolute_time(); 
        for(int i = 0; i < 1000; i++) {
            f_add = f1+f2;
        }
        float t_add = (to_us_since_boot(get_absolute_time()) - to_us_since_boot(t))/1000.0/0.006667;
        printf("\nAdd Cycles = %f\n", t_add);


        t = get_absolute_time(); 
        for(int i = 0; i < 1000; i++) {
            f_sub = f1-f2;
        }
        float t_sub = (to_us_since_boot(get_absolute_time()) - to_us_since_boot(t))/1000.0/0.006667;
        printf("Sub Cycles = %f\n", t_sub);

        t = get_absolute_time(); 
        for(int i = 0; i < 1000; i++) {
            f_mult = f1*f2;
        }
        float t_mult = (to_us_since_boot(get_absolute_time()) - to_us_since_boot(t))/1000.0/0.006667;
        printf("Mult Cycles = %f\n", t_mult);

        t = get_absolute_time(); 
        for(int i = 0; i < 1000; i++) {
            f_div = f1/f2;
        }
        float t_div = (to_us_since_boot(get_absolute_time()) - to_us_since_boot(t))/1000.0/0.006667;
        printf("Div Cycles = %f\n", t_div);

        
        printf("\nResults: \n%f + %f = %f \n%f - %f = %f \n%f * %f = %f \n%f / %f = %f\n", f1,f2,f_add, f1,f2,f_sub, f1,f2,f_mult, f1,f2,f_div);
        sleep_ms(1000);
    }
}
