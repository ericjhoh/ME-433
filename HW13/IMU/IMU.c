#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h"


// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 8
#define I2C_SCL 9

#define ADDR 0x68


// config registers
#define CONFIG 0x1A
#define GYRO_CONFIG 0x1B
#define ACCEL_CONFIG 0x1C
#define PWR_MGMT_1 0x6B
#define PWR_MGMT_2 0x6C
// sensor data registers:
#define ACCEL_XOUT_H 0x3B
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40
#define TEMP_OUT_H   0x41
#define TEMP_OUT_L   0x42
#define GYRO_XOUT_H  0x43
#define GYRO_XOUT_L  0x44
#define GYRO_YOUT_H  0x45
#define GYRO_YOUT_L  0x46
#define GYRO_ZOUT_H  0x47
#define GYRO_ZOUT_L  0x48
#define WHO_AM_I     0x75

static int16_t values[7];
static float real_values[7];

void IMU_init();
void read_data();
void combine_bytes(unsigned char data[]);
void print_info();
void draw_arrows();


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

    while(!stdio_usb_connected()){}

    unsigned char who_reg = WHO_AM_I;
    //  check what value is returned from who am i register
    i2c_write_blocking(I2C_PORT, ADDR, &who_reg, 1, true);
    unsigned char who;
    i2c_read_blocking(I2C_PORT, ADDR, &who, 1, false);

    printf("WHO_AM_I: 0x%2x\n", who);

    if(who != 0x68) {
        printf("I2C bus is not working or chip not plugged in\n");
        gpio_init(25);           // Initialize the LED pin
        gpio_set_dir(25, GPIO_OUT);  // Set as output

        while(true){
            gpio_put(25, 1);      
            sleep_ms(1000);            
            gpio_put(25, 0);     
            sleep_ms(1000);           
        }
    }

    IMU_init();
    ssd1306_setup();
    ssd1306_clear();
    ssd1306_update();


    while(true) {
        read_data();
        print_info();

        draw_arrows();
        ssd1306_update();

        sleep_ms(10); // 100 hz cycle
        
    }



}

void draw_arrows() {
    ssd1306_clear();

    float x_accel = real_values[0];
    float y_accel = real_values[1];

    //x --> 128 pixels
    int num_pixels_x = (int)(64.0 * fabsf(x_accel));

    //y --> 32 pixels
    int num_pixels_y = (int)(16.0 * fabsf(y_accel));


    if(y_accel > 0) {
        for(int i = 0; i < num_pixels_y; i++) {
            ssd1306_drawPixel(64, 16+i, 1);
        }
    }
    else {
        for(int i = 0; i < num_pixels_y; i++) {
            ssd1306_drawPixel(64, 16-i, 1);
        }
    }


     if(x_accel > 0) {
        for(int i = 0; i < num_pixels_x; i++) {
            ssd1306_drawPixel(64-i, 16, 1);
        }
    }
    else {
        for(int i = 0; i < num_pixels_x; i++) {
            ssd1306_drawPixel(64+i, 16, 1);
        }
    }



}

void IMU_init() {

    unsigned char buf[2];
    buf[0] = PWR_MGMT_1;
    buf[1] = 0x00;  // turns on the IMU chip
    i2c_write_blocking(I2C_PORT, ADDR, buf, 2, false); 

    buf[0] = ACCEL_CONFIG;
    buf[1] = 0x00;  // set accel sense to +- 2g
    i2c_write_blocking(I2C_PORT, ADDR, buf, 2, false); 


    buf[0] = GYRO_CONFIG;
    buf[1] = 0x18;  // sets gyro to +- 2000 dps
    i2c_write_blocking(I2C_PORT, ADDR, buf, 2, false); 
}


void read_data() {
    unsigned char start = ACCEL_XOUT_H;

    // check what value is returned from who am i register
    i2c_write_blocking(I2C_PORT, ADDR, &start, 1, true);
    unsigned char raw_val[14];
    i2c_read_blocking(I2C_PORT, ADDR, raw_val, 14, false);

    combine_bytes(raw_val);

    // acceleration: gs
    real_values[0] = 0.000061 * values[0];
    real_values[1] = 0.000061 * values[1];
    real_values[2] = 0.000061 * values[2];

    // temperature: Celcius
    real_values[3] = values[3]/340.00 + 36.53;

    // gyro:  degrees/sec
    real_values[4] = 0.007630 * values[4];
    real_values[5] = 0.007630 * values[5];
    real_values[6] = 0.007630 * values[6];
}

void combine_bytes(unsigned char data[]) {

    int count = 0;
    for(int i = 0; i < 14; i += 2) {
        values[count] = (int16_t)((data[i] << 8) | data[i+1]);
        count++;
    }

}

void print_info() {

    printf("\t\t ACCEL\t\t GYRO\n");
    printf("X:\t\t %.2f\t\t %.2f\n", real_values[0], real_values[4]);
    printf("Y:\t\t %.2f\t\t %.2f\n", real_values[1], real_values[5]);
    printf("Z:\t\t %.2f\t\t %.2f\n", real_values[2], real_values[6]);
    printf("\nTemp: %.2f C\n", real_values[3]);
}

