#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "printf_tools.h"

#define lineRight PB0
#define lineLeft PB1
#define lineCenter PB2

#define leftMotor PB3
#define rightMotor PD3

void configureIO(){
    DDRD &= ~(1 << lineRight);
    DDRD &= ~(1 << lineCenter);
    DDRD &= ~(1 << lineLeft);

    printf("I/O Configured!");
}

void configurePWM(){
    /*
     *  COM2A1 and COM2B1 : Clear when count reaches Compare Match
     *  WGM : PWM Mode
     */
    TCCR2A |= (1 << COM2A1) | (1 << COM2B1) | (1 << WGM20) | (1 << WGM21);
    TCCR2B |= (1 << WGM22) | (1 << CS20);
    TCCR2A = 0; // Normal Mode
}

void setLeftMotor(uint8_t percentageLoad){
    OCR2A = percentageLoad;
}

void setRightMotor(uint8_t percentageLoad){
    OCR2B = percentageLoad;
}

int main(){
    init_printf_tools();

    configureIO();
    configurePWM();

    while(1){
        
    }
}