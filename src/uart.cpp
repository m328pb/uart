#include "uart.h"
#include <avr/interrupt.h>
#include <avr/io.h>

UART::UART() {}

void UART::init(void) {
  uint8_t sreg = SREG;
  cli(); // disable interrupts globally, just in case..

  // set UART baud rate generator for asynchronous mode
  // (p. 146 ATmega328p datasheet)
  UBRR0H = (uint8_t)(UBRR >> 8);
  UBRR0L = (uint8_t)UBRR;

  if (!U2X)
    UCSR0A &= ~(1 << U2X0); // single speed mode
  else
    UCSR0A |= (1 << U2X0); // double speed mode

  UCSR0B = (1 << TXEN0);                  // Enable UART transmitter
  UCSR0B &= ~(1 << RXEN0);                // Disable UART receiver
  UCSR0B &= ~(1 << UDRIE0);               // disable interrupts
  UCSR0B &= ~(1 << TXCIE0);               // disable TX complete interrupt
  UCSR0B &= ~(1 << RXCIE0);               // disable RX complete interrupt
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8 data bits
                                          // all other zero in UCSR0C mea
  // UMSEL01, UMSEL00 - asynchronous USART
  // UPM01,UPM00 - parity disabled
  // USBS0 - 1-bit stop
  // USPOL0 - clock polarity, not used in asynchronous mode

  SREG = sreg;
}

/*
 * @brief send byte of data
 * @param data - byte to send
 */
void UART::send(char data) {
  // Wait for empty transmit buffer
  if (!USE_INTERRUPT) {
    // do not use interrupts, just block and wait for TX register empty
    do {
    } while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = data; // send data
  } else {
    // use interrupts to not block
    // will skip data when buffer overflow with speed in mind
    uint8_t next = (buffer_head + 1) % BUFFER_SIZE;
    if (next == buffer_tail) { // buffer full: drop
      buffer[buffer_head] = '@';
    } else {
      buffer[buffer_head] = data;
      buffer_head = next;
    }
    UCSR0B |= (1 << UDRIE0); // enable interrupt
  }
}

/*
 * @brief send char string
 * @params str - pointer to string to send. MUST terminate with 0x0
 */
void UART::send_ln(const char *str) {
  while (*str) {
    send(*str++);
  }
  send('\n');
}

void UART::isr() {
  // function to be called from Interrupt vector
  // ISR(USART0_UDRE_vect){
  // 	uart.isr();
  // }
  if (buffer_head == buffer_tail) {
    UDR0 = buffer[buffer_tail];
    buffer_tail = (buffer_tail + 1) % BUFFER_SIZE;
  } else {
    // Turn off UART Data Register Empty interrupt
    UCSR0B &= ~(1 << UDRIE0);
  }
}

void UART::off() {
  // Wait for empty transmit buffer
  do {
  } while (!(UCSR0A & (1 << UDRE0)));
  UCSR0B &= ~(1 << TXEN0); // Disable UART transmitter
}
