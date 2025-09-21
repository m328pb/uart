# uart

minimalistic UART interface for ATmega328p/pb for transmission only. Basicaly
copy of example from datasheet.

## main features

- included `library.json` so can be easily attached to other project with
platformio library manager. Just add to `platformio.ini` the line:

```bash
lib\_deps = https://github.com/m328pb/uart
```

- minimum implementation to make it lightweight (no Arduino libs dependency)
ARDUINO sketch takes 1724bytes vs 538bytes of example using this lib.

```bash
AVR Memory Usage
----------------
Device: Unknown

Program:     450 bytes
(.text + .data + .bootloader)

Data:          0 bytes
(.data + .bss + .noinit)
```

- only asynhronous transmission, 8bit+1bit stop, no parity check
- only single speed so pay atention on baud selection to not exceed
error limit (~2%). To caluclate error: calculate UBRR from baud rate,
round down and calculate back the baud rate:

$$
\begin{aligned}
UBRR=\frac{F_{OSC}}{16 \cdot BAUD}+1
\qquad\qquad
BAUD=\frac{F_{OSC}}{16 \cdot (UBRR+1)}
\end{aligned}
$$

following 9600bps 19200bps 38400bps 76800bps works fine for 16MHz

- on ATmega328pb uses only USART0
- library do not use interrupts, just loop until register can accept new data.
So for low baud rates it's relatively slow. So for example if you want to write
to serial everytime you send byte through I2C...expect pauses ;)

## usage

Library provides class UART::UART() with following methods:

- UART::init() - initialize chip registers for UART communication with default
baud rate (9600bps)
- UART::init(uint32_t baud) - initialize chip registers for UART communication
with selected baud rate
- UART::send(char data) - sends single byte of data
- UART::send_ln(const char *data) - sends string (MUST be ended with 0), finish
with new line char.
- UART::off()

## example

Script sends sample text, just use network analyzer or see in terminal. Simply
compile examples/main.cpp ([env:demo] in platformio.ini).
