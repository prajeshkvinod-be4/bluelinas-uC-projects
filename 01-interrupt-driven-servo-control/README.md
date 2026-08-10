#### Interrupt-Driven Servo Control using STM32F103

**Author:** Prajesh K V
**Target Board:** STM32F103C8T6 (Blue Pill)

#### Overview

A bare-metal C project for the STM32F103C8T6 (Blue Pill) implementing event-driven, low-power servo motor control. The system utilizes hardware timers for PWM signal generation, EXTI external interrupts for button input detection, centralized state evaluation, and the ARM Cortex-M `__WFI()` instruction for CPU power optimization.

This project was developed strictly using CMSIS register-level access without relying on HAL or SPL libraries, referencing the STM32F103 Reference Manual (RM0008) and Datasheet.

#### Hardware Specifications

* Microcontroller: STM32F103C8T6 Blue Pill (ARM Cortex-M3 @ 72 MHz)
* Actuator: SG90 Micro Servo
* Inputs: 2 × Tactile Push Buttons
* Indicator: 1 × Status LED

#### Pin Mapping

| STM32 Pin | Function           | Configuration                             |
| --------- | ------------------ | ----------------------------------------- |
| PA0       | TIM2 Channel 1 PWM | Alternate Function Push-Pull (50 MHz)     |
| PA1       | Status LED Output  | General Purpose Push-Pull (2 MHz)         |
| PA2       | Button 1 Input     | Input with Internal Pull-Up (EXTI Line 2) |
| PA3       | Button 2 Input     | Input with Internal Pull-Up (EXTI Line 3) |

#### Key Features

* **Direct Register Access:** Register-level peripheral configuration using CMSIS structures (RCC, GPIOA, TIM2, AFIO, EXTI, NVIC).
* **Interrupt-Driven Architecture:** Replaced CPU-bound polling with edge-triggered EXTI lines.
* **Truth-Table State Machine:** Centralized hardware state evaluation using `Update_Servo_State()` to handle multi-button states.
* **Low Power Operation:** Main loop executes `__WFI()` (Wait For Interrupt), placing the Cortex-M core into sleep mode until a hardware event occurs.
* **Full Pulse Range Calibration:** Pulse widths from 500 µs to 2500 µs were used for SG90 servo positioning.

#### State Truth Table

| Button 1 (PA2) | Button 2 (PA3) | System State        | Servo Position | CCR1 Value | LED State |
| -------------- | -------------- | ------------------- | -------------- | ---------- | --------- |
| Released       | Released       | Idle / Neutral      | 90°            | 1500 µs    | OFF       |
| Pressed        | Released       | Button 1 Active     | 0°             | 500 µs     | ON        |
| Released       | Pressed        | Button 2 Active     | 180°           | 2500 µs    | ON        |
| Pressed        | Pressed        | Dual-Press Conflict | 90°            | 1500 µs    | ON        |

#### System Architecture & Interrupt Flow

```text
[ Push Button Action ]
          |
          v
( PA2 / PA3 Pin Transition )
          |
          v
  [ AFIO Multiplexer ]
          |
          v
 [ EXTI Line 2 / Line 3 ]  <-- Dual Edge Triggers (FTSR + RTSR)
          |
          v
   [ ARM Cortex NVIC ]
          |
          v
 [ EXTI2 / EXTI3 ISR ]     <-- Wakes CPU from __WFI() Sleep
          |
          v
 Update_Servo_State()       <-- Reads GPIOA->IDR & Evaluates Truth Table
          |
          v
 Set TIM2->CCR1 & PA1      <-- Updates Servo & LED, then returns to sleep
```

#### PWM & Timer Math

TIM2 is configured to generate a standard 50 Hz (20 ms frame) RC servo signal:

* **System Clock:** 72 MHz
* **Prescaler Register (`TIM2->PSC`):** 71

```text
Timer Counter Frequency
= 72 MHz / (71 + 1)
= 1 MHz
= 1 µs per timer tick
```

* **Auto-Reload Register (`TIM2->ARR`):** 19999

```text
PWM Frame Period
= (19999 + 1) × 1 µs
= 20 ms
= 50 Hz
```

#### Duty Cycle Calculation

* **0° Position:** `CCR1 = 500` ticks → 0.5 ms pulse
* **90° Position:** `CCR1 = 1500` ticks → 1.5 ms pulse
* **180° Position:** `CCR1 = 2500` ticks → 2.5 ms pulse

#### Power Management

The main execution loop uses the Cortex-M intrinsic sleep instruction:

```c
while (1)
{
    __WFI();
}
```

The CPU waits in a low-power sleep state while hardware peripherals continue operating. When an EXTI rising or falling edge occurs on PA2 or PA3, the NVIC wakes the core to service the corresponding interrupt handler.

#### Software Setup & Build Instructions

1. **Toolchain:** Keil MDK / ARM-GCC with CMSIS headers
2. **Flash Tool:** ST-Link V2

#### Concepts & Skills Demonstrated

* Technical documentation navigation (STM32 Reference Manual RM0008, Datasheet, and Pinout diagrams)
* Mapping register bit-fields directly to hardware peripheral behavior
* Bare-metal ARM Cortex-M architecture
* Hardware timer PWM configuration
* External Interrupt Controller (EXTI) configuration
* AFIO pin multiplexing
* Nested Vectored Interrupt Controller (NVIC) integration
