#pragma once
#include <avr/interrupt.h>
#include <avr/io.h>

#ifndef BAUD
#define BAUD 250000
// there is no x2 mode so pay atention choosing the baud
// calculate integer of UBRR and back to baud to see the error
// try to be below 2%
// see p.146 ATmega328P datasheet for formulas
#endif

#ifndef F_CPU
#define F_CPU 16000000L
#endif

// set UART baud rate generator for asynchronous mode
// (p. 146 ATmega328p datasheet)
#define BAUD_RATE_REG (F_CPU/16/BAUD-1)

void initUART(void);
void printUART(char data);
void printlnUART(const char *str);
