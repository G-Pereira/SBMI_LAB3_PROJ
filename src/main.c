#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "printf_tools.h"

#define lineRight PA5
#define lineCenterRight PA2
#define lineCenter PA3
#define lineCenterLeft PA0
#define lineLeft PA1

#define motorLeftPWM PE3
#define motorLeftDigital PB5

#define motorRightPWM PE4
#define motorRightDigital PH5

#define encoderRightC1 PE4
#define encoderRightC2 PE5
#define encoderLeftC1 PD2
#define encoderLeftC2 PD3

#define FASTFORWARD 2
#define SLOWFORWARD 1
#define STOP 0
#define SLOWBACKWARD (-1)
#define FASTBACKWARD (-2)

void configureIO() {
    // Line Sensors
    DDRA &= ~(1 << lineRight);
    DDRA &= ~(1 << lineCenterRight);
    DDRA &= ~(1 << lineCenter);
    DDRA &= ~(1 << lineCenterLeft);
    DDRA &= ~(1 << lineLeft);

    // Motors
    DDRE |= (1 << motorLeftPWM);
    DDRB |= (1 << motorLeftDigital);
    DDRE |= (1 << motorRightPWM);
    DDRH |= (1 << motorRightDigital);
}

void configurePWM() {
    /*
     *  COM2A1 and COM2B1 : Clear when count reaches Compare Match
     *  WGM : PWM Mode
     */
    TCCR3A |= (1 << COM3A1) | (1 << COM3B1) | (1 << WGM30) | (1 << WGM31);
    TCCR3B |= (1 << CS20);
}

void setRightMotor(uint8_t velocity) {
    if (velocity > 0) {
        OCR3A = (velocity / 2) * 255;
        PORTB &= ~(1 << motorLeftDigital);
    } else if (velocity < 0) {
        OCR3A = 255 - (velocity / 2) * 255;
        PORTB |= (1 << motorLeftDigital);
    }

}

void setLeftMotor(uint8_t velocity) {
    if (velocity > 0) {
        OCR3B = (velocity / 2) * 255;
        PORTH &= ~(1 << motorRightDigital);
    } else if (velocity < 0) {
        OCR3B = 255 - (velocity / 2) * 255;
        PORTH |= (1 << motorRightDigital);
    }
}

int main() {
    init_printf_tools();

    configureIO();
    configurePWM();

    uint8_t left, leftCenter, center, rightCenter, right;

    while (1) {
        left = PINA && (1 << lineLeft) ? 1 : 0;
        leftCenter = PINA && (1 << lineCenterLeft) ? 1 : 0;
        center = PINA && (1 << lineCenter) ? 1 : 0;
        rightCenter = PINA && (1 << lineCenterRight) ? 1 : 0;
        right = PINA && (1 << lineRight) ? 1 : 0;

        setRightMotor(SLOWFORWARD);
        setLeftMotor(SLOWFORWARD); // logica destas funções está ao contrário, pois só anda no slow forward e não no fast

        /*if (!left && !leftCenter && !center && !rightCenter && !right){
            setRightMotor(STOP);
            setLeftMotor(STOP);
        }
        else if ((leftCenter && center && rightCenter)||(!rightCenter && center && !leftCenter)) {
            setRightMotor(FASTFORWARD);
            setLeftMotor(FASTFORWARD);
        } else if (rightCenter && ! right){
            setRightMotor(SLOWFORWARD);
            setLeftMotor(FASTFORWARD);
        } else if (leftCenter && ! left){
            setRightMotor(FASTFORWARD);
            setLeftMotor(SLOWFORWARD);
        } else if (!leftCenter && left){
            setRightMotor(FASTFORWARD);
            setLeftMotor(STOP);
        } else if (!rightCenter && right){
            setRightMotor(STOP);
            setLeftMotor(FASTFORWARD);
        }*/
    }
}