#pragma once
#include <avr/interrupt.h>
#include <avr/io.h>

#define BAUD 9600U // default baud rate

class UART {
private:
  uint32_t baud = BAUD;

public:
  UART();
  void init(void);
  void init(uint32_t baud);
  void send(char data);
  void send_ln(const char *str);
  void off();
};
