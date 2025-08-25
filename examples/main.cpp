/*
 simply routine to check and demonstarte the library
  - plug in network analyzer
  - check recived text with network analyzer
 */
#include "uart.h"

int main() {
  initUART();
  printlnUART("test_UART");
  return 0;
}
