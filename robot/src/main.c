#include <avr/io.h>
#include <avr/interrupt.h>
#include "rc5.h"

#define lineRight PH1
#define lineCenterRight PH0
#define lineCenter PJ0
#define lineCenterLeft PJ1
#define lineLeft PB7

#define motorLeftPWM PL3
#define motorLeftDigital PL5
#define motorRightPWM PL4
#define motorRightDigital PL2

#define powerButton PD3

#define VREF 5

#define TOP 1000
#define T4COUNT 65536-31125 // 0,125s

#ifndef F_CPU
#define F_CPU 16000000ul
#endif
#define BAUD 57600
#define UBBR_VAL ((F_CPU/(BAUD*16))-1) // Calculate UBBR value based on desired baudrate

uint8_t volatile time=0;

void toggleHeadLights(char state){
    if (state){
        PORTC |= (1 << PC0);
        PORTC |= (1 << PC1);
        PORTG |= (1 << PG2);
        PORTD |= (1 << PD7);
    } else{
        PORTC &= ~(1 << PC0);
        PORTD &= ~(1 << PD7);
        PORTG &= ~(1 << PG2);
        PORTC &= ~(1 << PC1);
    }
}

void configureUSART(){
    UBRR0 = UBBR_VAL;
    UCSR0B = 1 << TXEN0; // Enable Transmitter (TX)
    UCSR0C = 3 << UCSZ00; // Data frame lengthtime
}

void configureIO() {
    // Line Sensors
    DDRH &= ~(1 << lineRight);
    DDRH &= ~(1 << lineCenterRight);
    DDRJ &= ~(1 << lineCenter);
    DDRJ &= ~(1 << lineCenterLeft);
    DDRB &= ~(1 << lineLeft);

    // Motors
    DDRL |= (1 << motorLeftPWM);
    DDRL |= (1 << motorLeftDigital);
    DDRL |= (1 << motorRightPWM);
    DDRL |= (1 << motorRightDigital);

    // Head Lights
    DDRC |= (1 << PC0);
    DDRD |= (1 << PD7);
    DDRG |= (1 << PG2);
    DDRC |= (1 << PC1);

    // Power Button
    DDRD &= ~(1 << powerButton);

    // Luminosity Indicator
    DDRE |= (1 << PE4);
    DDRE |= (1 << PE5);
    DDRG |= (1 << PG5);
    DDRB |= (1 << PB4);
    DDRE |= (1 << PE3);
    DDRH |= (1 << PH3);
    DDRH |= (1 << PH4);
    DDRH |= (1 << PH5);
    DDRH |= (1 << PH6);
    DDRB |= (1 << PB5);
}

void contigureTimer(){
	CLKPR = 0x80; // Set CLKPCE, clear other bits
	CLKPR = 3; //clock prescaler: 8
	TCCR4B = 0; // Stop TC1
	TIFR4 |= (7 << TOV1); // Clear pending intr
	TCCR4A = 0; // Normal mode
	TCNT4 = T4COUNT; // Load BOTTOM value
	TIMSK4 = (1<<TOIE4); // Enable OVF interrupt
	TCCR4B = 2;  //time prescaler: 8
}

ISR(TIMER4_OVF_vect){
	time++;
	TCNT4=T4COUNT;
}

/*void configureSleepMode(){
    SMCR = (1 << SM1);
}*/

void configurePWM() {
    /*
     *  COM5A1 and COM5B1 : Clear when count reaches Compare Match
     *  WGM : PWM Mode
     */
    ICR5 = TOP;
    TCCR5A |= (1<<COM5A1) | (1<<COM5B1) | (1<<WGM51);
    TCCR5B |= (1<<WGM52) | (1<<WGM53) | (1<<CS51);
}

void configureADC(){
    // Definir Vref=AVcc
    ADMUX = ADMUX | (1<<REFS0);
    // Pré-divisor em 128 e ativar ADC
    ADCSRA |= (7<<ADPS0)|(1<<ADEN);
}

unsigned int readBattery(){
	ADMUX = (ADMUX & 0xF0) | (1 << MUX0);
	ADCSRA |= (1 << ADSC);
	while(ADCSRA & (1<<ADSC));
    showBattery((ADC-245)/10); // número de 0-10
	return ADC;
}

unsigned int readPhotoResistor(){
    ADMUX = (ADMUX & 0xF0);
    ADCSRB = 0;
    ADCSRA |= (1 << ADSC);
    while(ADCSRA & (1<<ADSC));
    return ADC;
}

void setMotors(int linearVelocity, int angularVelocity){
    int right, left;
    left = linearVelocity + angularVelocity;
    right = linearVelocity - angularVelocity;
    if (right<0){
        OCR5A = TOP+right;
        PORTL |= (1 << motorRightDigital);
    } else{
        OCR5A = right;
        PORTL &= ~(1 << motorRightDigital);
    }
    if (left<0){
        OCR5B = TOP+left;
        PORTL |= (1 << motorLeftDigital);
    } else{
        OCR5B = left;
        PORTL &= ~(1 << motorLeftDigital);
    }
}

// Dividir por 10 ao chamar a função 0-10
void showBattery(uint8_t percentage){
    switch(percentage){
        case 10:
            PORTE |= (1 << PE4);
        case 9:
            PORTB |= (1 << PB5);
        case 8:
            PORTB |= (1 << PB4);
        case 7:
            PORTH |= (1 << PH6);
        case 6:
            PORTH |= (1 << PH5);
        case 5:
            PORTH |= (1 << PH4);
        case 4:
            PORTH |= (1 << PH3);
        case 3:
            PORTE |= (1 << PE3);
        case 2:
            PORTG |= (1 << PG5);
        case 1:
            PORTE |= (1 << PE5);
        break;
    }
    switch(percentage){
        case 0:
            PORTE &= ~(1 << PE5);
        case 1:
            PORTG &= ~(1 << PG5);
        case 2:
            PORTE &= ~(1 << PE3);
        case 3:
            PORTH &= ~(1 << PH3);
        case 4:
            PORTH &= ~(1 << PH4);
        case 5:
            PORTH &= ~(1 << PH5);
        case 6:
            PORTH &= ~(1 << PH6);
        case 7:
            PORTB &= ~(1 << PB4);
        case 8:
            PORTB &= ~(1 << PB5);
        case 9:
            PORTE &= ~(1 << PE4);
        case 10:
        break;
    }
}

void debugPrint(char print){
    while(0==(UCSR0A&(1<<UDRE0)));
    UDR0 = print;
}

int main() {
    configureUSART();
    configureIO();
    configurePWM();
    configureADC();
    /*RC5_Init();
    sei();*/

    uint8_t left, leftCenter, center, rightCenter, right;
    uint8_t state = 0;
    uint8_t on=0;
    uint8_t laps = 0;
    uint16_t command;

    while (1) {
        // Read Inputs
        on = PIND & (1 << powerButton) ? 1 : 0;
        left = PINB & (1 << lineLeft) ? 1 : 0;
        leftCenter = PINJ & (1 << lineCenterLeft) ? 1 : 0;
        center = PINJ & (1 << lineCenter) ? 1 : 0;
        rightCenter = PINH & (1 << lineCenterRight) ? 1 : 0;
        right = PINH & (1 << lineRight) ? 1 : 0;

        // State Machine for moving logic
        if(0 == state && on){
            state = 1;
        } else if (1 == state && !on){
            laps = 0;
            time = 0;
            state = 3;
        } else if( 0 != state && 1 != state && on){
            state = 2;
        } else if(2 == state && !on){
            state = 0;
        } else if(3 == state && rightCenter){
            state = 4;
        } else if(3 == state && leftCenter){
            state = 6;
        } else if(4 == state && right){
            state = 5;
        } else if (6 == state && left){
            state = 7;
        } else if (4 == state && center){
            state = 3;
        } else if (5 == state && rightCenter){
            state = 4;
        } else if(6 == state && center){
            state = 3;
        } else if (7 == state && leftCenter){
            state = 6;
        }else if (6 == state && rightCenter){
            state = 4;
        }else if (4 == state && leftCenter){
            state = 6;
        }else if (7 == state && rightCenter){
            state = 4;
        }else if (5 == state && leftCenter){
            state = 6;
        }

        //setMotors(0, 500);
        /*if ( 0 == state || 1 == state)
            setMotors(0, 0);
        if (3 == state)
            setMotors(600, 0);
        if (4 == state)
            setMotors(400, 250);
        if (5 == state)
            setMotors(300, 350);
        if (6 == state)
            setMotors(400, -250);
        if (7 == state)
            setMotors(300, -350);*/

        /*if(RC5_NewCommandReceived(&command)){
            RC5_Reset();
            debugPrint('b');
            if(RC5_GetStartBits(command) != 3)
            {
                 ERROR
            }
            
            uint8_t toggle = RC5_GetToggleBit(command);
            uint8_t address = RC5_GetAddressBits(command);
            uint8_t cmdnum = RC5_GetCommandBits(command);
            debugPrint('a');
        }*/

        readBattery();
        if(readPhotoResistor()<250)
            toggleHeadLights(1);
        else
            toggleHeadLights(0);

        if(time>=120 && right && left){
        	laps++;
        	time=0;
        }
        
        if(laps==3){
        	laps=0;
        	state=0;
        }
    }
}