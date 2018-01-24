#include <avr/io.h>

#define lineRight PA5
#define lineCenterRight PA2
#define lineCenter PA3
#define lineCenterLeft PA0
#define lineLeft PA1

#define motorLeftPWM PL1
#define motorLeftDigital PL2
#define motorRightPWM PL4
#define motorRightDigital PL3

#define powerButton PG1

#define VREF 5

#define TOP 1000

#ifndef F_CPU
#define F_CPU 16000000ul
#endif
#define BAUD 57600
#define UBBR_VAL ((F_CPU/(BAUD*16))-1) // Calculate UBBR value based on desired baudrate

void toggleHeadLights(char state){
    if (state){
        PORTG |= (1 << PG0);
        PORTL |= (1 << PL7);
        PORTL |= (1 << PL6);
        PORTL |= (1 << PL5);
    } else{
        PORTG &= ~(1 << PG0);
        PORTL &= ~(1 << PL7);
        PORTL &= ~(1 << PL6);
        PORTL &= ~(1 << PL5);
    }
}

void configureUSART(){
    UBRR1 = UBBR_VAL;
    UCSR1B = 1 << RXEN1; // Enable Receiver (RX)
    UCSR1C = 3 << UCSZ10; // Data frame length
}

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

    // Head Lights
    DDRG |= (1 << PG0);
    DDRL |= (1 << PL7);
    DDRL |= (1 << PL6);
    DDRL |= (1 << PL5);

    // Power Button
    DDRG |= (1 << powerButton);

    // Luminosity Indicator
    DDRE |= (1 << PE4);
    DDRE |= (1 << PE5);
    DDRG |= (1 << PG5);
    DDRE |= (1 << PE3);
    DDRH |= (1 << PH3);
    DDRH |= (1 << PH4);
    DDRH |= (1 << PH5);
    DDRH |= (1 << PH6);
    DDRB |= (1 << PB4);
    DDRB |= (1 << PB5);
}

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
    // Desativar buffer digital em PC0
    DIDR0 |= (1<<ADC4D);
    // Pré-divisor em 128 e ativar ADC
    ADCSRA |= (7<<ADPS0)|(1<<ADEN); 
}

uint16_t readPhotoResistor(){
    ADMUX |= (1 << MUX2);
    ADCSRA |= (1 << ADSC);
    showLuminosity(ADC/10); // número de 0-10
    while(ADCSRA & (1<<ADSC));
    return ADC;
}

void setMotors(int linearVelocity, int angularVelocity){
    if(linearVelocity<0){
        OCR5A = TOP+linearVelocity+angularVelocity;
        PORTL |= (1 << motorLeftDigital);
        OCR5B = TOP+linearVelocity-angularVelocity;
        PORTL |= (1 << motorRightDigital);
    } else {
        OCR5A = linearVelocity-angularVelocity;
        PORTL &= ~(1 << motorLeftDigital);
        OCR5B = linearVelocity+angularVelocity;
        PORTL &= ~(1 << motorRightDigital);
    }
}

uint8_t receiveRemote(){
    if (!(UCSR1A | (1 << RXC1))) return 0;
    if (UDR1 == 'f') return 1;
    if (UDR1 == 'b') return 2;
    if (UDR1 == 'l') return 3;
    if (UDR1 == 'r') return 4;
    return 0;
}

// Dividir por 10 ao chamar a função 0-10
void showLuminosity(uint8_t percentage){
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
            PORTE |= (1 << PE5);
        case 1:
            PORTG |= (1 << PG5);
        case 2:
            PORTE |= (1 << PE3);
        case 3:
            PORTH |= (1 << PH3);
        case 4:
            PORTH |= (1 << PH4);
        case 5:
            PORTH |= (1 << PH5);
        case 6:
            PORTH |= (1 << PH6);
        case 7:
            PORTB |= (1 << PB4);
        case 8:
            PORTB |= (1 << PB5);
        case 9:
            PORTE |= (1 << PE4);
        case 10:
        break;
    }
}

int main() {
    configureUSART();
    configureIO();
    configurePWM();
    configureADC();

    uint8_t left, leftCenter, center, rightCenter, right;
    uint8_t state=0;
    uint8_t on=0;
    uint16_t lightIntensity;

    while (1) {
        // Read Inputs
        on = PING & (1 << powerButton) ? 1 : 0;
        left = PINA & (1 << lineLeft) ? 1 : 0;
        leftCenter = PINA & (1 << lineCenterLeft) ? 1 : 0;
        center = PINA & (1 << lineCenter) ? 1 : 0;
        rightCenter = PINA & (1 << lineCenterRight) ? 1 : 0;
        right = PINA & (1 << lineRight) ? 1 : 0;

        // State Machine for moving logic
        if(0 != receiveRemote()){
            state = 7;
        } else if(0 == state && on){
            state = 1;
        } else if (1 == state && !on){
            state = 2;
        } else if( 0 != state && 1 != state && on){
            state = 0;
        } else if(2 == state && rightCenter){
            state = 3;
        } else if(2 == state && leftCenter){
            state = 5;
        } else if(3 == state && right){
            state = 4;
        } else if (5 == state && left){
            state = 6;
        } else if (3 == state && center){
            state = 2;
        } else if (4 == state && rightCenter){
            state = 3;
        } else if(5 == state && center){
            state = 2;
        } else if (6 == state && leftCenter){
            state = 5;
        }

        if ( 0 == state || 1 == state){
            setMotors(0, 0);
        }
        if (1 == state)
            setMotors(500,0);
        if (2 == state)
            setMotors(400*0.8, 120);
        if (3 == state)
            setMotors(0, 400);
        if (4 == state)
            setMotors(400*0.8, -120);
        if (5 == state)
            setMotors(0, -400);

        // Remote Logic
        if (7 == state){
            switch (receiveRemote()){
                case 1:
                setMotors(400, 0);
                break;
                case 2:
                setMotors(-400, 0);
                break;
                case 3:
                setMotors(0, 200);
                break;
                case 4:
                setMotors(400, -200);
                break;
                default:
                setMotors(0, 0);
                break;
            }
        }

        lightIntensity = readPhotoResistor();

        if(lightIntensity<200)
            toggleHeadLights(1);
        else
            toggleHeadLights(0);
    }
}