#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "ssd1306.h"
#include "font.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 8
#define I2C_SCL 9

void drawLetter(int x, int y, char c);
void drawMessage(int x, int y, char * m);


int main()
{
    stdio_init_all();

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c


    adc_init();
    adc_gpio_init(26); 
    adc_select_input(0);

    ssd1306_setup();
    ssd1306_clear();
    ssd1306_update();

    while (true) {

        unsigned int start = to_us_since_boot(get_absolute_time());  
        char message[50];
        char fps_message[50];

        uint16_t raw = adc_read();
        float voltage = raw * 3.3 / 4095.0;


        sprintf(message, "Voltage: %.2f V", voltage);
        drawMessage(0,0,message); //x, y, pointer to array
        ssd1306_update();

        unsigned int end = to_us_since_boot(get_absolute_time());  
        
        float fps =  1.0e6 / ((float)(end-start));

        sprintf(fps_message, "FPS: %.2f", fps);
        drawMessage(0,24,fps_message); //x, y, pointer to array
        ssd1306_update();


    }
}


void drawMessage(int x, int y, char * m) {
    int i = 0;
    while(m[i] != 0) {
        drawLetter(x + i*5, y, m[i]);
        i++;
    }
}


void drawLetter(int x, int y, char c){
    int row, col;
    row = c - 0x20;
    col = 0;
    for(col = 0; col < 5; col++) {
        char byte = ASCII[row][col];

        for(int i = 0; i < 8; i++){
            char on_or_off = (byte >> i) & 0b00000001;
            ssd1306_drawPixel(x + col,y + i, on_or_off);
        }
    }
}