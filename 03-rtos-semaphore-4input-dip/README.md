# ESP32-C3 FreeRTOS Binary Semaphore - DIP Switch LED Control

A small embedded project using **ESP32-C3, FreeRTOS, and 4 position DIP switch** to trigger LED blinking through binary semaphore synchronization.

## Features

- GPIO input configuration + internal pull-up resistors
- FreeRTOS tasks for DIP code checking and LED control
- Binary semaphore for communication between tasks
- LED blinks five times when the expected DIP code is detected
- ESP-IDF logging for switch states and system messages

## Circuit



## Hardware

- ESP32-C3 SuperMini
- 4-position DIP switch
- LED with current-limiting resistor (if using an external LED)
- Breadboard and jumper wires

## Tech Stack

`C` · `ESP-IDF` · `FreeRTOS` · `Binary Semaphore` · `GPIO`

## Key Learning

Implemented task synchronization using a FreeRTOS binary semaphore, with GPIO-based input monitoring, conditional triggering, LED control, and ESP-IDF logging.
