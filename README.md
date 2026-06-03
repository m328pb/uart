# uart

Minimalistic UART interface for ATmega328p/pb for transmission only. Basically
copy of example from datasheet.

## main features

- included `library.json` so can be easily attached to other project with
platformio library manager. Just add to `platformio.ini` the line:

```center_code
lib\_deps = https://github.com/m328pb/uart
```

- minimum implementation to make it lightweight (no Arduino libs dependency,
no interrupts). ARDUINO sketch takes 1724 bytes vs 262 bytes of example using
this lib.

```pad_code
AVR Memory Usage
----------------
Device: Unknown

Program:     388bytes
(.text + .data + .bootloader)

Data:          0 bytes
(.data + .bss + .noinit)
```

```pad_code
AVR Memory Usage (interrupt version)
----------------
Device: Unknown

Program:     796bytes
(.text + .data + .bootloader)

Data:          0 bytes
(.data + .bss + .noinit)
```

- only asynchronous transmission, 8bit+1bit stop, no parity check
- uses only USART0
- library do not use interrupts by default, just loop ('blocks') until
TX register can accept new data. For low baud rates it's relatively slow.
For example, if you want to write to serial every time you send byte
through I2C...expect pauses ;)
- Pay attention also when heavily using 'blocking' UART version in connection with
counter interrupt; I observed some glitches for ISR counter incremented variable
(incremented much more cycles then expected), most likely blocked by UART;
Happened with these conditions after few hundreds cycles: cycle every 0.2 sec,
sending ~120chars @9600 bps (0.1 sec). As a rule of thumb: UART sending time
should be kept below 1/10 of cycle you use.
- For above reason, 'interrupt' option can be added with USE_INTERRUPT set to 1.
Interrupt implementation use ring buffer; its size can be set with BUFFER_SIZE,
default is 100bytes. This come with significant memory cost!
In addition to normal class usage, it is necessary to call
UART.isr() function from within interrupt vector routine (class instance must
be global...obviously):

```pad_code
ISR(USART_UDRE_vect){
    uart.isr();
}
```

## usage

Library provides class UART::UART() with following methods. To set baud
change #define UART_BAUD_RATE in uart.h or use compiler flag (by default
UART_BAUD_RATE = 115200).

- UART::init() - initialize chip registers for UART communication with default
baud rate (9600bps), or other BAUD if defined
- UART::send(char data) - sends single byte of data
- UART::send_ln(const char *data) - sends string (MUST be ended with 0), finish
with new line char.
- UART::buffer_empty() - return 1 when all data from buffer send
- UART::isr() - function to be called from within Interrupt Routine (ISR)
- UART::off()

## example

Script sends sample text, just use network analyzer or see in terminal. Simply
compile examples/main.cpp ([env:demo] in platformio.ini).
