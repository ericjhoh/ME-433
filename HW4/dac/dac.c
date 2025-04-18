#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"

// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_CS   17
#define PIN_SCK  18
#define PIN_MOSI 19
#define RESOLUTION 1024

static inline void cs_select(uint cs_pin) {
    asm volatile("nop \n nop \n nop"); // FIXME
    gpio_put(cs_pin, 0);
    asm volatile("nop \n nop \n nop"); // FIXME
}

static inline void cs_deselect(uint cs_pin) {
    asm volatile("nop \n nop \n nop"); // FIXME
    gpio_put(cs_pin, 1);
    asm volatile("nop \n nop \n nop"); // FIXME
}


void writeDac(int channel, float voltage);

int main()
{
    stdio_init_all();

    // SPI initialisation. This example will use SPI at 1MHz.
    spi_init(SPI_PORT, 20000000);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS,   GPIO_FUNC_SIO);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    
    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);
    // For more examples of SPI use see https://github.com/raspberrypi/pico-examples/tree/master/spi


    while (true) {
        
        float t = 0;
        float v_sin = 0;
        float v_tri = 0;
        for(int i = 0; i < 100; i++) {

            // writes the voltage of the sin wave output A
            v_sin = 1.65 * (sin( 4 * M_PI * t) + 1);
            writeDac(0, v_sin);

            //writes the voltage of the triangle wave to output B
            if(i < 50) {
                v_tri += 3.3/50; 
            }
            else {
                v_tri -= 3.3/50;
            }
            writeDac(1, v_tri);
            

            //increments time
            t = t + .01;
            sleep_ms(.01);
        }


    }
}


void writeDac(int channel, float voltage) {

    uint8_t data[2];
    int len = 2;

    data[0] = 0b01110000 | (channel << 7);
    data[1] = 0b00000000;
    uint16_t v = voltage * RESOLUTION / 3.3;
    v = v & 0x3FF; // ensures v is a 10 bit number


    data[0] = data[0] | (v>>6);
    data[1] = data[1] | ((v & 0x3F) << 2);

    cs_select(PIN_CS);
    spi_write_blocking(SPI_PORT, data, len); // where data is a uint8_t array with length len
    cs_deselect(PIN_CS);
}