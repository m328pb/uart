// Test program. Will be run on simavr simulator
// at 9600bps, one char takes 0.8ms
// with click every 10ms should send 12 chars

#include "uart.h"
#include <avr/interrupt.h>
#include <util/delay.h>

// use port for debugging status
// simavr will rise IRQ when change
// use IOPORT_IRQ_REG_PORT, not single pin
// UART TX & RX pins are on PORTD
#define STATUS_DBG PORTB
#define CLICK_DBG PORTC

UART serial;
volatile uint16_t click = 0;

int main() {
  uint16_t click_start = 0;
  // initiate timer interrupt every 0.001sec
  TCCR1B = (1 << WGM12);               // CTC
  TCCR1B |= (1 << CS11) | (1 << CS10); // prescaler: 64
                                       // TOP value: 245,000=1sec
  OCR1A = ((F_CPU / 64) - 1) / 1000;   // 0.001 sec=1ms
  TIMSK1 |= (1 << OCIE1A);

  serial.init();

  STATUS_DBG = 1; // TEST1: send complete text,
                  // check time it takes for each iteration
  for (uint8_t i = 0; i < 10; i++) {
    CLICK_DBG = 0; // IRQ is catch only when status change
                   // so will not fire if loop takes the
                   // same time
                   // for each iteration
    click_start = click;
    serial.send_ln(
        "test UART with interrupts: 9600bps"); // 34 char + start,stop and 0x00
    while (!serial.buffer_empty())
      ; // wait until lib deactivate TX interrupt
    CLICK_DBG = click - click_start;
  };
  STATUS_DBG = 2; // TEST1 finished

  STATUS_DBG = 3; // TEST2: buffer overflow
  serial.send_ln("a lot of text to exceed buffer size; buffer is 40 char");
  STATUS_DBG = 4; // TEST2 finished

  // FINISH
  serial.off();
  STATUS_DBG = 5;
  return 0;
}

ISR(USART0_UDRE_vect) { serial.isr(); }
ISR(TIMER1_COMPA_vect) { click++; }
