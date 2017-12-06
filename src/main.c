#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "printf_tools.h"

#define lineRight PB4
#define lineLeft PD7
#define lineCenter PB0

#define motorLeftPWM PB3
#define motorLeftDigital PD4 
#define motorLeftEnable PD6

#define motorRightPWM PD3
#define motorRightDigital PD5
#define motorRightEnable PB5

#define FASTFORWARD 2
#define SLOWFORWARD 1
#define STOP 0
#define SLOWBACKWARD (-1)
#define FASTBACKWARD (-2)

void configureIO(){
  // Line Sensors
  DDRB &= ~(1 << lineRight);
  DDRD &= ~(1 << lineCenter);
  DDRB &= ~(1 << lineLeft);
  
  // Motors
  DDRB |= (1 << motorLeftPWM);
  DDRD |= (1 << motorLeftDigital);
  DDRB |= (1 << motorLeftEnable);
  DDRD |= (1 << motorRightPWM);
  DDRD |= (1 << motorRightDigital);
  DDRB |= (1 << motorRightEnable);

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