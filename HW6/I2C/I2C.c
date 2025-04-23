#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 4
#define I2C_SCL 5

#define ADDR 0b00100000

#define IODIR  0b00000000
#define GPIO  0b00001001
#define OLAT 0b00001010


void setPin(unsigned char address, unsigned char register, unsigned char value);
unsigned char readPin(unsigned char address, unsigned char register);

int main()
{
    stdio_init_all();

    while(!stdio_usb_connected()){}

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/
    
    //set GP7 as output and GP0 as input
    setPin(ADDR, IODIR, 0b01111111);

    //set GP7 output as low
    setPin(ADDR, OLAT, 0b00000000);

    while (true) {

        // Read GP0 
        unsigned char input = readPin(ADDR, GPIO);

        // check whether GP0 is on or off
        if ((input & 0b00000001) == 0) {
            // turn on GP7 button pressed
            setPin(ADDR, OLAT, 0b10000000);
        } else {
            // turn off GP7 button not pressed
            setPin(ADDR, OLAT, 0b00000000);
        }
    }
}


void setPin(unsigned char address, unsigned char reg, unsigned char value) {

    unsigned char buf[2];
    buf[0] = reg;
    buf[1] = value;
    i2c_write_blocking(I2C_PORT, address, buf, 2, false);
}

unsigned char readPin(unsigned char address, unsigned char reg) {
    i2c_write_blocking(I2C_PORT, address, &reg, 1, true);
    unsigned char buf;
    i2c_read_blocking(I2C_PORT, address, &buf, 1, false);
    return buf;
}
