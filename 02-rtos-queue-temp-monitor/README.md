ESP32-C3 FreeRTOS Temperature Monitor

A simple temperature-monitoring project using an ESP32-C3, an NTC thermistor, and FreeRTOS.

Features

Reads the thermistor using the ADC.

Calculates temperature using the Beta equation.

Uses a FreeRTOS queue to transfer sensor readings between tasks.

Blinks an LED when the temperature reaches the configured threshold.

Uses ESP-IDF logging for sensor and system messages.

Hardware

ESP32-C3

10K NTC thermistor

10K fixed resistor

LED and current-limiting resistor

Breadboard and jumper wires

Software

C

ESP-IDF

FreeRTOS

ADC and ADC calibration APIs

What I Learned

Practiced FreeRTOS tasks, queues, ADC readings, temperature calculation, GPIO control, and ESP-IDF logging.
