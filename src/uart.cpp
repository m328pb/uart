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
                                          // Interrupt version will enable
                                          // interrupts ONLY when data in buffer
  UCSR0B &= ~(1 << TXCIE0);               // disable TX complete interrupt
  UCSR0B &= ~(1 << RXCIE0);               // disable RX complete interrupt
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8 data bits
                                          // all other zero in UCSR0C mea
  // UMSEL01, UMSEL00 - asynchronous USART
  // UPM01,UPM00 - parity disabled
  // USBS0 - 1-bit stop
  // USPOL0 - clock polarity, not used in asynchronous mode

  SREG = sreg;
  if (USE_INTERRUPT)
    sei();
}

/*
 * @brief send byte of data
 * @param data - byte to send
 */
void UART::send(char data) {
  if (!USE_INTERRUPT) {
    // do not use interrupts, just block and wait for TX register empty
    while (!(UCSR0A & (1 << UDRE0)))
      ;
    UDR0 = data; // send data

  } else {               // use interrupts to not block
    if (buffer_full()) { // buffer full: drop
      cli();
      // move_head_back(8);
      buffer_head = buffer_tail;
      const char *info = "!--overflow--!"; // try to leave info
      while (*info) {
        buffer_push(*info++);
      };
      sei();
    } else {
      buffer_push(data);
    }
    UCSR0B |= (1 << UDRIE0); // enable interrupt
  }
}

//
// @brief send char string
// @params str - pointer to string to send. MUST terminate with 0x0
//
void UART::send_ln(const char *str) {
  while (*str) {
    send(*str++);
  }
  send('\n');
}

// function to be called from Interrupt vector
// ISR(USART0_UDRE_vect){
// 	uart.isr();
// }
void UART::isr() {
  if (!buffer_empty()) // data in the buffer
    buffer_pop();
  else
    UCSR0B &= ~(1 << UDRIE0); // buffer is empty
                              // Turn off UART Data Register Empty interrupt
}

// add element to buffer and adjust head
void UART::buffer_push(char c) {
  if (buffer_full())
    buffer[buffer_head] = c;
  else {
    uint8_t next = (buffer_head + 1) % BUFFER_SIZE;
    buffer[buffer_head] = c;
    buffer_head = next;
  }
}

// send element from buffer and adjust tail
void UART::buffer_pop(void) {
  UDR0 = buffer[buffer_tail];
  buffer_tail = (buffer_tail + 1) % BUFFER_SIZE;
}

// return 1 if buffer is empty
uint8_t UART::buffer_empty() {
  if (!USE_INTERRUPT)
    return 1;
  if (buffer_head == buffer_tail) {
    return 1; // empty
  } else {
    return 0; // not empty
  }
}

// return 1 if buffer is full
uint8_t UART::buffer_full(void) {
  uint8_t next = (buffer_head + 1) % BUFFER_SIZE;
  if (next == buffer_tail) {
    return 1; // buffer full
  } else {
    return 0; // not full
  }
}

// turn off UART interface
// send all data before shutting down
void UART::off() {
  if (USE_INTERRUPT) {
    // Wait for empty transmit buffer
    while (!buffer_empty())
      ;
  } else {
    // or just send last char
    do {
    } while (!(UCSR0A & (1 << UDRE0)));
  }
  UCSR0B &= ~(1 << TXEN0); // Disable UART transmitter
}
