# STM32F769 RFID Door Lock

An STM32F7xx-compatible RFID-RC522 project and library using FreeRTOS (CMSISv2)
as a base, UART for serial communication / debugging, SPI for communication
with the reader and a servo motor for simulating a door lock.

## Running the project

1. Have STM32CubeIDE installed
2. Clone the project: `git clone https://github.com/erazemk/stm32f7-mfrc522.git`
3. Launch `.project` (this will import the project into STM32CubeIDE)

## Connecting the RFID-RC522 module

The following is the default pinout, but you can change it in the code if needed.

| RFID-RC522 | STM32F769NI |
|:----------:|:-----------:|
|  SDA (SS)  |     D10     |
| SCK (SCLK) |     D13     |
|    MOSI    |     D11     |
|    MISO    |     D12     |
|     IRQ    |      /      |
|     GND    |     GND     |
|     RST    |     3.3V    |
|    3.3V    |     3.3V    |

## Connecting the Servo motor

| SG90 | STM32F769NI |
|:----:|:-----------:|
| VCC  |     5V      |
| DATA |     D6      |
| GND  |     GND     |

## Debugging using UART (Serial Port)

The board automatically prints useful info to serial over UART.
To read it use a program like `tio` (or `screen` or PuTTY) to connect to
the board's USB port (usually `COMx` on Windows and `/dev/ttyACMx` on Linux).

With a program like `tio` the defaults are enough (so `tio /dev/ttyACM0`),
but here are the specifics:

- Baud rate: 115200
- Data size: 8 bits
- Parity: None
- Stop bits: 1

## Attribution

This project uses a custom MFRC522 library, parts of which were taken from or
inspired by the following libraries:

- [Miguel Balboa's Arduino RFID library](https://github.com/miguelbalboa/rfid)
- [Tilen Majerle's tm_stm32f4_mfrc522 library](https://github.com/MaJerle/stm32f429)
- [Mojca Rojko's rfid-rc522 library](https://github.com/xtrinch/stm32f7-demos/tree/master/07-rfid-rc522)
