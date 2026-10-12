# ESP32-C3 FreeRTOS Binary Semaphore — DIP Switch LED Control

An embedded project using **ESP32-C3, FreeRTOS, and 4 position DIP switch** to trigger LED blinking through binary semaphore synchronization.

## Features

- GPIO input configuration with internal pull-up resistors
- FreeRTOS tasks for DIP code checking and LED control
- Binary semaphore for communication between tasks
- LED blinks five times when the expected DIP code is detected
- ESP-IDF logging for switch states and system messages

## Circuit

[ESP32-C3 DIP Switch Circuit 1](https://github.com/prajeshkvinod-be4/bluelinas-uC-projects/blob/main/03-rtos-semaphore-4input-dip/Circuit_dip_1.png) ([image](https://github.com/prajeshkvinod-be4/bluelinas-uC-projects/raw/main/03-rtos-semaphore-4input-dip/Circuit_dip_1.png))

[ESP32-C3 DIP Switch Circuit 2](https://github.com/prajeshkvinod-be4/bluelinas-uC-projects/blob/main/03-rtos-semaphore-4input-dip/Circuit_dip_2.png) ([image](https://github.com/prajeshkvinod-be4/bluelinas-uC-projects/raw/main/03-rtos-semaphore-4input-dip/Circuit_dip_2.png))

## Hardware

- ESP32-C3 
- 4-position DIP switch
- LED with current-limiting resistor (if using an external LED)
- Breadboard and jumper wires

## Tech Stack

`C` · `ESP-IDF` · `FreeRTOS` · `Binary Semaphore` · `GPIO`

## Key Learning

Implemented task synchronization using a FreeRTOS binary semaphore, with GPIO-based input monitoring, conditional triggering, LED control, and ESP-IDF logging.
