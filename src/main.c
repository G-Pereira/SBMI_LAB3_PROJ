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

#define motorLeftPWM PL1
#define motorLeftDigital PL2
#define motorRightPWM PL4
#define motorRightDigital PL3

#define encoderRightC1 PD0
#define encoderRightC2 PD1
#define encoderLeftC1 PD2
#define encoderLeftC2 PD3

#define FASTFORWARD 300
#define SLOWFORWARD 250
#define STOP 0
#define SLOWBACKWARD -SLOWFORWARD
#define FASTBACKWARD -FASTFORWARD

/*#ifndef F_CPU
#define F_CPU 16000000ul
#endif
#define BAUD 57600
#define UBBR_VAL ((F_CPU/(BAUD*16))-1) // Calculate UBBR value based on desired baudrate
*/
void toggleHeadLights(char state){
    if (state){
        PORTE |= (1 << PE4);
        PORTE |= (1 << PE5);
        PORTG |= (1 << PG5);
        PORTE |= (1 << PE3);
    } else{
        PORTE &= ~(1 << PE4);
        PORTE &= ~(1 << PE5);
        PORTG &= ~(1 << PG5);
        PORTE &= ~(1 << PE3);
    }
}

/*void configureUSART(){
    UBRR0 = UBBR_VAL;
    UCSR0B = 1 << TXEN0; // Enable Transmitter (TX)
    UCSR0C = 3 << UCSZ00; // Data frame length
}*/

void configureIO() {
    // Line Sensors
    DDRA &= ~(1 << lineRight);
    DDRA &= ~(1 << lineCenterRight);
    DDRA &= ~(1 << lineCenter);
    DDRA &= ~(1 << lineCenterLeft);
    DDRA &= ~(1 << lineLeft);

    // Motors
    DDRL |= (1 << motorLeftPWM);
    DDRL |= (1 << motorLeftDigital);
    DDRL |= (1 << motorRightPWM);
    DDRL |= (1 << motorRightDigital);

    // Encoders
    DDRD|=(1<<encoderLeftC2)|(1<<encoderLeftC1);
    DDRD|=(1<<encoderRightC2)|(1<<encoderRightC1);

    // Head Lights
    DDRE |= (1 << PE4);
    DDRE |= (1 << PE5);
    DDRG |= (1 << PG5);
    DDRE |= (1 << PE3);
}

void configurePWM() {
    /*
     *  COM5A1 and COM5B1 : Clear when count reaches Compare Match
     *  WGM : PWM Mode
     */
    ICR5 = 1000;
    TCCR5A |= (1<<COM5A1) | (1<<COM5B1) | (1<<WGM51);
    TCCR5B |= (1<<WGM52) | (1<<WGM53) | (1<<CS51);
}

void setRightMotor(uint16_t velocity) {
        OCR5B = velocity;
        PORTL &= ~(1 << motorRightDigital);
}

void setLeftMotor(uint16_t velocity) {
        OCR5A = velocity;
        PORTL &= ~(1 << motorLeftDigital);
}

/*void showTurn(uint64_t revs){
    while(0==(UCSR1A&(1<<UDRE1)));
    UDR1 = revs;
}*/

int main() {

    //configureUSART();
    configureIO();
    configurePWM();
    toggleHeadLights(0);

    uint8_t left, leftCenter, center, rightCenter, right, linePosition=0, enRC1, enRC2;
    uint64_t revRight=0;

    while (1) {
        left = PINA & (1 << lineLeft) ? 0 : 1;
        leftCenter = PINA & (1 << lineCenterLeft) ? 0 : 1;
        center = PINA & (1 << lineCenter) ? 0 : 1;
        rightCenter = PINA & (1 << lineCenterRight) ? 0 : 1;
        right = PINA & (1 << lineRight) ? 0 : 1;

        linePosition = 2*left + leftCenter + rightCenter + 2*right;

        /*enRC1 = (PIND & (1<<encoderLeftC1))?1:0;
        enRC2 = (PIND & (1<<encoderLeftC2))?1:0;

        if(state==0 && enRC1 && !enRC2){
        	revRight++;
        	state=1;
        }
        else if(state==1 && !enRC1 && !enRC2){
        	revRight--;
        	state=0;
        }
        else if(state==1 && enRC1 && enRC2){
        	revRight++;
        	state=2;
        }
        else if(state==2 && enRC1 && !enRC2){
        	revRight--;
        	state=1;
        }
        else if(state==2 && !enRC1 && enRC2){
        	revRight++;
        	state=3;
        }
        else if(state==3 && enRC1 && enRC2){
        	revRight--;
        	state=2;
        }
        else if(state==3 && !enRC1 && !enRC2){
            revRight++;
            state=0;
        }
        else if(state == 0 && !enRC1 && enRC2){
        	revRight--;
        	state = 3;
        }
        showTurn(revRight);*/
    }
}