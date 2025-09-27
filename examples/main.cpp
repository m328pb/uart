/*
 simply routine to check and demonstarte the library
  - plug in network analyzer or simply use any terminal
 */
#include "uart.h"
#include <util/delay.h>
int main() {
  UART serial;
  while (1) {
    serial.init();
    serial.send_ln("test UART: 115200bps");
    _delay_ms(1000);
  }
  serial.off();
  return 0;
}
