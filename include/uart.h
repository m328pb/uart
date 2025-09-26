#pragma once
#include <avr/interrupt.h>
#include <avr/io.h>

#define BAUD 115200 // define baud rate
                    // max 1Mbps

// calculate if multiplier needed based on baud error
// try with single speed
#define U2X 0
#define UBRR ((F_CPU / (16UL*BAUD)) - 1)
#define BAUD_CALC (F_CPU / (16UL * (UBRR + 1)))
// to avoid negative numbers shift by 100
#define BAUD_ERROR (200 - ((BAUD * 100UL) / BAUD_CALC))
#if (BAUD_ERROR > 102) | (BAUD_ERROR < 98)
  // error greater then 2%, use double speed
  #undef UBRR
  #define UBRR ((F_CPU / (8UL*BAUD)) - 1)
  #undef U2X
  #define U2X 1 // double speed
#endif

class UART {
public:
  UART();
  void init(void);
  void send(char data);
  void send_ln(const char *str);
  void off();
};
