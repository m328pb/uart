/*
 Used to measure memory usage: env:mem_usage in platformio.ini
 then do avr-size on firmware.elf
 */
#include "uart.h"
int main() {
  initUART();
  printUART(0);
  printlnUART("");
  return 0;
}
