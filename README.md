# uart_m328pb

minimalistic UART interface for ATmega328p/pb for transmission only. Basicaly
copy of example from datasheet.

## main features

- included `library.json` so can be easily attached to other project with
platformio library manager. Just add to `platformio.ini` the line:

```bash
lib_deps = https://github.com/zielaskowski/i2c
```

- minimum implementation to make it lightweight

```bash
AVR Memory Usage
----------------
Device: Unknown

Program:     228 bytes
(.text + .data + .bootloader)

Data:          0 bytes
(.data + .bss + .noinit)
```

- no Arduino libs dependency
- only asynhronous transmission, 8bit+1bit stop, no parity check
- only single speed so pay atention on baud selection to not exceed
error limit (~2%). Calculate UBRR from baud rate, round down and calculate
back the baud rate:

$$
UBRR=\frac{F_{OSC}}{16 \cdot BAUD}+1
BAUD=\frac{F_{OSC}}{16 \cdot (UBRR+1)}
$$

- on ATmega328pb uses only USART0
- library do not use interrupts, just loop until register can accept new data

## usage

Library provide just basic functions:

- initUART() - initialize chip registers for UART communication
- printUART(char data) - sends single byte of data
- printlnUART(const char *data) - sends string (MUST be ended with 0), finish
with new line char.

## example

Script sends sample text, just use network analyzer or see in terminal. Simply
compile examples/main.cpp ([env:demo] in platformio.ini).
