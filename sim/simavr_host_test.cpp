#include "simavr/avr_ioport.h"
#include "simavr/avr_uart.h"
#include "simavr/sim_avr.h"
#include "simavr/sim_elf.h"
#include "simavr/sim_irq.h"

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

static std::vector<char> UART_send;
static std::vector<int> time_v;

std::string UART_send_last;
std::string test1_str = "test UART with interrupts: 9600bps\n";

// call back function for UART interrupt
static void uart_cb(struct avr_irq_t *, uint32_t value, void *) {
  UART_send.push_back((char)value);
};

// call back function for PORTB interrupt (user defined as STATUS_DBG)
static void status_dbg_cb(struct avr_irq_t *irq, uint32_t value, void *param) {
  avr_t *avr = (avr_t *)param;
  switch (value) {
  case 1: {
    std::cout << "Started TEST1: expected behavior\n";
    break;
  }
  case 2: { // TEST1 finish
    assert(UART_send_last == test1_str);
    float ave_time = 0;
    for (int c : time_v) {
      ave_time += c;
    }
    std::cout << "Finished TEST1: " << time_v.size() << " cycles ";
    std::cout << "in average " << ave_time / time_v.size() << " ms/cycle\n";
    break;
  }
  case 3: {
    std::cout << "Started TEST2: buffer overflow\n";
    break;
  }
  case 4: { // TEST2 finished
    std::cout << "Finished TEST2\n";
    break;
  }
  case 5: {
    std::cout << "UART turned off\n";
    std::string s(UART_send.begin(),UART_send.end());
    assert(s.find("!--overflow--!") != std::string::npos);
    avr->state = cpu_Stopped;
    break;
  }
  }
}

// call back function for PORTC interrupt (user defined as CLICK_DBG)
static void click_dbg_cb(struct avr_irq_t *irq, uint32_t value, void *param) {
  if (value == 0)
    return;
  std::string s(UART_send.begin(), UART_send.end());
  UART_send_last = s;
  UART_send.clear();
  time_v.push_back(value);
}

int main() {
  avr_t *avr = avr_make_mcu_by_name("atmega328pb");
  // load binary file to simulate
  elf_firmware_t fw{};
  elf_read_firmware(FIRMWARE_ELF, &fw);
  avr_init(avr);
  avr_load_firmware(avr, &fw);
  std::cout << "Loaded: " << FIRMWARE_ELF << '\n';

  // UART IRQ event observer
  avr_irq_t *uart_irq =
      avr_io_getirq(avr, AVR_IOCTL_UART_GETIRQ('0'), UART_IRQ_OUTPUT);
  avr_irq_register_notify(uart_irq, uart_cb, nullptr);

  // PORTB IRQ (STATUS_DBG) event observer
  avr_irq_t *status_irq =
      avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('B'), IOPORT_IRQ_REG_PORT);
  avr_irq_register_notify(status_irq, status_dbg_cb, avr);

  // PORTC IRQ (CLICK_DBG) event observer
  avr_irq_t *click_irq =
      avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('C'), IOPORT_IRQ_REG_PORT);
  avr_irq_register_notify(click_irq, click_dbg_cb, nullptr);

  int state = cpu_Running;
  while (state == cpu_Running) {
    state = avr_run(avr);
  }
  avr_terminate(avr);
  std::cout << "Test passed!\n";
  return 0;
}
