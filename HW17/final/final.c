#include <stdio.h>
#include "pico/stdlib.h"
#include "cam.h"
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

#define START 20

#define WRAP 50000

void init_PWM_DIR() {
    gpio_set_function(PWMA, GPIO_FUNC_PWM); // Set the LED Pin to be PWM
    gpio_set_function(PWMB, GPIO_FUNC_PWM); // Set the LED Pin to be PWM

    gpio_init(DIRA);
    gpio_init(DIRB);

    gpio_init(START);
    gpio_set_dir(START, GPIO_IN);

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



int main()
{

    // initialize pins
    stdio_init_all();
    init_camera_pins();
    init_PWM_DIR();


    // wait until start button is pressed
    while(gpio_get(START) != 0) {}

    while (true) {

        // gets the image
        setSaveImage(1);
        while(getSaveImage()==1){}
        convertImage();
        
        // returns the column that represents the center of mass of the 
        // brightest pixels in the middle row of the picture
        int com = findLine(IMAGESIZEY / 2);

        // same as above but the row 25% from the top --> looks ahead
        int com_top = findLine(IMAGESIZEY / 4); 

        // IMAGESIZEX --> 80 pixel wide is max --> center is 40 
        int offset = com - IMAGESIZEX/2; // the displacement the line is from the center
        // if negative --> line is to the left of the robot --> turn left
        // if positive --> line is to the right of the robot --> turn right


        int change = com_top - com; // change in the center of mass
        // if negative turn left
        // if positive turn right


        // max offset = 40

        // printf("Com: %d\nOffset:%d\n\n", com, offset);
        // printf("Com top: %d\nOffset:%d\n\n", com_top, offset);
        // printf("COM: %d\nCom_top: %d\n\n", com, com_top);

        // B is right
        // A is left
        // move straight
        if(offset > -5 && offset < 5) {
            pwm_set_gpio_level(PWMA, WRAP / 2); 
            pwm_set_gpio_level(PWMB, WRAP / 2);
        }
        else {
            // turn left --> decrease speed of left
            
            if(offset < 0) {
                pwm_set_gpio_level(PWMB, WRAP / 4);
                if(offset < -30) {
                    pwm_set_gpio_level(PWMA, WRAP / 8);
                }
                else {
                    float duty = .25 - (1.0/8.0/25.0)*(-1*offset - 5);
                    pwm_set_gpio_level(PWMA, WRAP * duty);
                }
            }
            //turn right --> decrease speed of right
            else {
                pwm_set_gpio_level(PWMA, WRAP / 4); 
                 if(offset > 30) {
                    pwm_set_gpio_level(PWMB, WRAP / 8);
                }
                else {
                    float duty = .25 - (1.0/8.0/25.0)*(offset - 5);
                    pwm_set_gpio_level(PWMB, WRAP * duty);
                }

            }
        }


        // // turn left --> decrease speed of left
        // if(offset <= -10 && offset >= -30) {
        //     pwm_set_gpio_level(PWMA, WRAP / 2);

        // }
        // // turn right --> increase speed of left
        // else if(offset >= 10 && offset <= 30) {
        //     pwm_set_gpio_level(PWMA, WRAP / 2); // set the duty cycle to 50%

        // }
        // // go straight
        // // max speeds
        // else {
        //     pwm_set_gpio_level(PWMA, WRAP / 2); // set the duty cycle to 50%
        //     pwm_set_gpio_level(PWMB, WRAP / 2);

        // }


        
        
    


        // // uncomment these and printImage() when testing with python 
        // char m[10];
        // scanf("%s",m);

        // setSaveImage(1);
        // while(getSaveImage()==1){}
        // convertImage();
        // int com = findLine(IMAGESIZEY/2); // calculate the position of the center of the line
        // setPixel(IMAGESIZEY/2,com,0,255,0); // draw the center so you can see it in python
        // printImage();
        // //printf("%d\r\n",com); // comment this when testing with python
    }
}



