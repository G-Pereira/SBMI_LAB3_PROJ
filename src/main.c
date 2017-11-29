#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "printf_tools.h"

#define lineRight1 PB0
#define lineRight2 PD7
#define lineCenter PD6
#define lineLeft1 PD5
#define lineLeft2 PD4

void configureIO(){
    DDRD &= ~(1 << lineRight2);
    DDRD &= ~(1 << lineRight1);
    DDRD &= ~(1 << lineCenter);
    DDRD &= ~(1 << lineLeft1);
    DDRD &= ~(1 << lineLeft2);

    printf("I/O Configured!");
}

int main(){
    init_printf_tools();

    configureIO();
}