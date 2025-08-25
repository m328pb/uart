#include "uart.h"
#include <avr/io.h>

void initUART(void) {
  uint8_t sreg = SREG;
  cli(); // disable interrupts globally, just in case..

  // asynchronous mode
  UBRR0H = (unsigned char)(BAUD_RATE_REG >> 8);
  UBRR0L = (unsigned char)(BAUD_RATE_REG);

  UCSR0A &= ~(1 << U2X0); // single speed mode

  UCSR0B = (1 << TXEN0);    // Enable UART transmitter
  UCSR0B &= ~(1 << RXEN0);  // Disable UART receiver
  UCSR0B &= ~(1 << UDRIE0); // disable interrupts
  UCSR0B &= ~(1 << TXCIE0);
  UCSR0B &= ~(1 << RXCIE0);
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8 data bits
  // all other zero in UCSR0C mean:
  // UMSEL01, UMSEL00 - asonchrynous USART
  // UPM01,UPM00 - parity disabled
  // USBS0 - 1-bit stop
  // USPOL0 - clock polarity, not used in asynchronous mode
  SREG = sreg;
}

void printUART(char data) {
  // Wait for empty transmit buffer
  while (!(UCSR0A & (1 << UDRE0)))
    ;
  UDR0 = data; // send data
}

void printlnUART(const char *str) {
  while (*str) {
    printUART(*str++);
  }
  printUART('\n');
}
