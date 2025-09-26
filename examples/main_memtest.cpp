/*
 Used to measure memory usage: env:mem_usage in platformio.ini
 then do avr-size on firmware.elf
 */
#include "uart.h"
int main() {
  UART serial;
  serial.init();
  serial.send(0);
  serial.send_ln("");
  serial.off();
  return 0;
}
