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

void configureIO() {
    // Line Sensors
    DDRB &= ~(1 << lineRight);
    DDRB &= ~(1 << lineCenter);
    DDRD &= ~(1 << lineLeft);

    // Motors
    DDRB |= (1 << motorLeftPWM);
    DDRD |= (1 << motorLeftDigital);
    DDRD |= (1 << motorLeftEnable);
    DDRD |= (1 << motorRightPWM);
    DDRD |= (1 << motorRightDigital);
    DDRB |= (1 << motorRightEnable);
}

void configurePWM() {
    /*
     *  COM2A1 and COM2B1 : Clear when count reaches Compare Match
     *  WGM : PWM Mode
     */
    TCCR2A |= (1 << COM2A1) | (1 << COM2B1) | (1 << WGM20) | (1 << WGM21);
    TCCR2B |= (1 << WGM22) | (1 << CS20);
    TCCR2A = 0; // Normal Mode
}

void setLeftMotor(uint8_t velocity) {
    if (velocity > 0) {
        OCR2A = (velocity / 2) * 255;
        PORTD &= ~(1 << motorLeftDigital);
    } else if (velocity < 0) {
        OCR2A = 255 - (velocity / 2) * 255;
        PORTD |= (1 << motorLeftDigital);
    }

}

void setRightMotor(uint8_t velocity) {
    if (velocity > 0) {
        OCR2B = (velocity / 2) * 255;
        PORTD &= ~(1 << motorRightDigital);
    } else if (velocity < 0) {
        OCR2B = 255 - (velocity / 2) * 255;
        PORTD |= (1 << motorRightDigital);
    }
}

int main() {
    init_printf_tools();

    configureIO();
    configurePWM();

    uint8_t left, right, center;

    while (1) {
        left = PIND && (1 << lineLeft) ? 1 : 0;
        right = PINB && (1 << lineRight) ? 1 : 0;
        center = PINB && (1 << lineCenter) ? 1 : 0;

        if (left == 1 && center == 1) {
            setRightMotor(FASTFORWARD);
            setLeftMotor(SLOWFORWARD);
        } else if (left == 1 && center == 0) {
            setRightMotor(FASTFORWARD);
            setLeftMotor(STOP);
        } else if (right == 1 && center == 1) {
            setRightMotor(SLOWFORWARD);
            setRightMotor(FASTFORWARD);
        } else if (right == 1 && center == 0) {
            setRightMotor(STOP);
            setLeftMotor(FASTFORWARD);
        } else if (right == 0 && left == 0 && center == 1) {
            setRightMotor(FASTFORWARD);
            setLeftMotor(FASTFORWARD);
        }
    }
}