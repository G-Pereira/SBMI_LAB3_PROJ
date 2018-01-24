#include <avr/io.h>

#ifndef F_CPU
#define F_CPU 16000000ul
#endif
#define BAUD 57600
#define UBBR_VAL ((F_CPU/(BAUD*16))-1) // Calculate UBBR value based on desired baudrate

void configureUSART(){
    UBRR0 = UBBR_VAL;
    UCSR0B = 1 << RXEN0; // Enable Transmitter (TX)
    UCSR0C = 3 << UCSZ00; // Data frame length
}

void configureIO(){
    
}

int main(){
    while (1){

    }
}