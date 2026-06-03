#pragma once
#include <avr/interrupt.h>
#include <avr/io.h>

#ifndef UART_BAUD_RATE
#define UART_BAUD_RATE 115200 // define baud rate; max 1Mbps
#endif

#ifndef USE_INTERRUPT
#define USE_INTERRUPT 0
#endif

#ifndef BUFFER_SIZE
// used only when USE_INTERRUPT equal 1
#define BUFFER_SIZE 100 // max size 256 (uint8_t)
#endif

// calculate if multiplier needed based on baud error
// try with single speed
#define U2X 0
#define UBRR ((F_CPU / (16UL * UART_BAUD_RATE)) - 1)
#define BAUD_CALC (F_CPU / (16UL * (UBRR + 1)))
// to avoid negative numbers shift by 100
#define BAUD_ERROR (200 - ((UART_BAUD_RATE * 100UL) / BAUD_CALC))
#if (BAUD_ERROR > 102) | (BAUD_ERROR < 98)
// error greater then 2%, use double speed
#undef UBRR
#define UBRR ((F_CPU / (8UL * UART_BAUD_RATE)) - 1)
#undef U2X
#define U2X 1 // double speed
#endif

class UART {
public:
  UART();
  void init(void);
  void send(char data);
  void send_ln(const char *str);
  void isr(void);
  uint8_t buffer_empty(void);
  uint8_t buffer_full(void);
  void off();

private:
  volatile char buffer[BUFFER_SIZE];
  volatile uint8_t buffer_head = 0;
  volatile uint8_t buffer_tail = 0;
  void buffer_pop(void);
  void buffer_push(char);
};
