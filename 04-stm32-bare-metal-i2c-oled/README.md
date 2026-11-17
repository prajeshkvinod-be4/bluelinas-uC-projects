# STM32F103 Bare-Metal I2C OLED Driver

A bare-metal embedded project using **STM32F103C8T6 (Blue Pill), I2C1, and an SSD1306 OLED display** to understand and implement I2C communication through direct peripheral register configuration.

## Features

- GPIOB configuration for I2C1 using alternate-function open-drain mode
- Direct register-level configuration of I2C1 peripheral
- I2C timing configuration for 400 kHz Fast Mode
- Implementation of I2C START and STOP conditions
- OLED address transmission and status flag polling
- Command and data transmission to the SSD1306 display
- OLED initialization, display clearing, and text output

## OLED Output

<img width="4096" height="1468" alt="Circuit_1" src="https://github.com/user-attachments/assets/2e8e6202-73f1-4c26-b25b-540ac62c42e7" />

## Hardware

- STM32F103C8T6 Blue Pill
- 0.96-inch SSD1306 I2C OLED display
- ST-Link V2 programmer/debugger
- Breadboard and jumper wires

## Tech Stack

`C` · `STM32F1` · `Bare-Metal Programming` · `I2C` · `GPIO Registers` · `SSD1306` · `Keil µVision`

## Key Learning

- Understood how I2C communication works between an STM32 and an external device.
- Learned to configure I2C from scratch using STM32 peripheral registers.
- Explored GPIO configuration, clock setup, and I2C timing registers using the STM32F103 reference manual and datasheet.
- Implemented START, STOP, address, command, and data transmission.
- Used trial and error to debug issues and get the OLED working.
- Gained practical experience with bare-metal programming in Keil without using an existing I2C library.
