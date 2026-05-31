/*
 simply routine to check and demonstrate the library
 Using interrupt to not block execution
 - plug in network analyzer or simply use any terminal
 */
#define USE_INTERRUPT 1

#include "uart.h"
#include <avr/interrupt.h>
#include <util/delay.h>

UART serial;

uint8_t pow(uint8_t expo) {
  int res = 1;
  for (uint8_t i = 0; i < expo; i++) {
    res *= 10;
  }
  return res;
}

int main() {
  serial.init();

  for (uint8_t i = 0; i < 100; i++) {
    uint8_t pos = 3;
    uint8_t val = i;
    uint8_t div;
    do {
      div = val / pow(pos - 1);
      val -= div * pow(pos - 1);
      serial.send('0' + div);
      pos--;
    } while (pos > 0);
    serial.send_ln(": test UART with interrupts: 115200bps");
    _delay_ms(1000);
  };
  serial.off();
  return 0;
}

ISR(USART0_UDRE_vect) { // NOLINT
  serial.isr();
}
