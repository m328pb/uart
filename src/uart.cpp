#include "uart.h"
#include <avr/io.h>

UART::UART(){}
/*
 * @brief initialize interface with given baud rate
 * @param baud - baud rate
 * @details
 * there is no x2 mode so pay atention choosing the baud:
 * calculate integer of UBRR and back to baud to see the error
 * try to be below 2% see p.146 ATmega328P datasheet for formulas
 * 9600bps 19200bps 38400bps 76800bps work fine for 16MHz
 */
void UART::init(uint32_t baud) {
  this->baud = baud;
  init();
}

void UART::init(void) {
  uint8_t sreg = SREG;
  cli(); // disable interrupts globally, just in case..

  // set UART baud rate generator for asynchronous mode
  // (p. 146 ATmega328p datasheet)
  uint16_t baud_rate_reg = (uint16_t)F_CPU / ((16 * baud) - 1);
  // asynchronous mode
  UBRR0H = (unsigned char)(baud_rate_reg >> 8);
  UBRR0L = (unsigned char)(baud_rate_reg);

  UCSR0A &= ~(1 << U2X0); // single speed mode

  UCSR0B = (1 << TXEN0);                  // Enable UART transmitter
  UCSR0B &= ~(1 << RXEN0);                // Disable UART receiver
  UCSR0B &= ~(1 << UDRIE0);               // disable interrupts
  UCSR0B &= ~(1 << TXCIE0);               // disable RX complete interrupt
  UCSR0B &= ~(1 << RXCIE0);               // disable TX complete interrupt
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8 data bits
  // all other zero in UCSR0C mean:
  // UMSEL01, UMSEL00 - asonchrynous USART
  // UPM01,UPM00 - parity disabled
  // USBS0 - 1-bit stop
  // USPOL0 - clock polarity, not used in asynchronous mode
  SREG = sreg;
}

void UART::send(char data) {
  // Wait for empty transmit buffer
  do {
  } while (!(UCSR0A & (1 << UDRE0)));
  UDR0 = data; // send data
}

void UART::send_ln(const char *str) {
  while (*str) {
    send(*str++);
  }
  send('\n');
}

void UART::off() {
  // Wait for empty transmit buffer
  do {
  } while (!(UCSR0A & (1 << UDRE0)));
  UCSR0B &= ~(1 << TXEN0); // Disable UART transmitter
}
