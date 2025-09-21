/*
 simply routine to check and demonstarte the library
  - plug in network analyzer
  - check recived text with network analyzer
 */
#include "uart.h"

int main() {
  UART serial;
  serial.init();
  serial.send_ln("test_UART: 9600bps");
  serial.init(76800);
  serial.send_ln("test UART: 76800bps");
  serial.off();
  return 0;
}
