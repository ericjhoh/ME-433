#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"

// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define SPI_PORT spi0
#define PIN_REC 16
#define PIN_CSDAC 17
#define PIN_SCK  18
#define PIN_SEND 19

#define PIN_CSRAM 20

union FloatInt {
    float f;
    uint32_t i;
};

void ram_write(uint16_t a, float v);
float ram_read (uint16_t a);

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

void init_ram() {

    uint8_t data[2];
    data[0] = 0b00000001;
    data[1] = 0b01000000; 

    cs_select(PIN_CSRAM);
    spi_write_blocking(SPI_PORT, data, 2);
    cs_deselect(PIN_CSRAM);
}

void writeDac(int channel, float voltage);


int main()
{
    stdio_init_all();

    while(!stdio_usb_connected()){
        sleep_ms(100);
    }

    // SPI initialisation. This example will use SPI at 1MHz.
    spi_init(SPI_PORT, 20000000);
    gpio_set_function(PIN_SEND, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CSDAC,   GPIO_FUNC_SIO);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_REC, GPIO_FUNC_SPI);

    gpio_set_function(PIN_CSRAM,   GPIO_FUNC_SIO);

    
    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_set_dir(PIN_CSDAC, GPIO_OUT);
    gpio_put(PIN_CSDAC, 1);

    gpio_set_dir(PIN_CSRAM, GPIO_OUT);
    gpio_put(PIN_CSRAM, 1);

    init_ram();

    // For more examples of SPI use see https://github.com/raspberrypi/pico-examples/tree/master/spi
    
    // loading 1000 floats representing a single sine wave cycle
    float v_sin = 0;
    for(int i = 0; i < 1000; i++) {

        // writes the voltage of the sin wave output A
        v_sin = 1.65 * (sin( 2 * M_PI * i / 1000.0) + 1);
        ram_write(i*4, v_sin);
    }

    int addr = 0;
    while(1) {

        v_sin = ram_read(addr*4);

        // output channel a of DAC
        writeDac(0, v_sin);
        if(addr == 999) {
            addr = 0;
        }
        else {
            addr++;
        }
        sleep_ms(1);

    }


}

void writeDac(int channel, float voltage) {

    uint8_t data[2];
    int len = 2;

    data[0] = 0b01110000 | (channel << 7);
    data[1] = 0b00000000;
    uint16_t v = voltage * 1024 / 3.3;
    v = v & 0x3FF; // ensures v is a 10 bit number


    data[0] = data[0] | (v>>6);
    data[1] = data[1] | ((v & 0x3F) << 2);

    cs_select(PIN_CSDAC);
    spi_write_blocking(SPI_PORT, data, len); // where data is a uint8_t array with length len
    cs_deselect(PIN_CSDAC);
}



void ram_write(uint16_t a, float v) {
    uint8_t data[7];

    //write
	data[0] = 0b00000010;
	
    //address
    data[1] = a >> 8; 
	data[2] = a & 0xFF;


    // value
    union FloatInt num;
	num.f = v;

    data[3] = num.i >> 24;
    data[4] = (num.i & 0x00FF0000) >> 16 ;
    data[5] = (num.i & 0x0000FF00) >> 8;
    data[6] = num.i & 0x000000FF;

	cs_select(PIN_CSRAM);
    spi_write_blocking(SPI_PORT, data, 7);
    cs_deselect(PIN_CSRAM);

}


float ram_read (uint16_t a) {

    uint8_t write[7], read[7];

    // writing in a specific address
    write[0] = 0b00000011;
    write[1] = a >> 8;
    write[2] = a & 0xFF;

    cs_select(PIN_CSRAM);
    spi_write_read_blocking(SPI_PORT, write, read, 7);
    cs_deselect(PIN_CSRAM);

    union FloatInt num;

    // processing the data of the read values.
    num.i = 0;
	num.i |= read[3] << 24;
    num.i |= read[4] << 16;
    num.i |= read[5] << 8;
    num.i |= read[6];
    
    // return voltage
    return num.f;
}