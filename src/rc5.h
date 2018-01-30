/* 
 * RC5 (36KHz Phillips protocol) Decoding library for AVR
 * Copyright (c) 2011 Filip Sobalski <pinkeen@gmail.com>
 * based on the idea presented by Guy Carpenter
 * on http://www.clearwater.com.au/rc5
 * 
 * Tested on ATmega328P. Designed for 16MHz crystal.
 * Should work on the ATmega{4/8/16/32}8 family 
 * without modification. Uses 16bit timer and an 
 * external interrupt.
 * 
 * I you use a different clock then adjust the timer
 * prescaler and pulse lengths accordingly.
 * 
 * Should be trivial to adapt to other AVRs sporting
 * a 16bit timer and an external interrupt. 
 * 
 */
#ifndef RC5_H
#define RC5_H

#include <stdint.h>



/* Initialize timer and interrupt */
void RC5_init();
void timer_init();


uint8_t RC5_GetToggleBit(uint16_t command);
uint8_t RC5_GetAddressBits(uint16_t command);
uint8_t RC5_GetCommandBits(uint16_t command);

/* Poll the library for new command.
 * 
 * You should call RC5_Reset immediately after
 * reading the new command because it's halted once 
 * receiving a full command to ensure you can read it
 * before it becomes overwritten. If you expect that only 
 * one remote at a time will be used then library
 * should be polled at least once per ~150ms to ensure
 * that no command is missed.
 */
uint16_t RC5_NewCommandReceived();


#endif