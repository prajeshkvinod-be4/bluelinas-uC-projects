
/*
 * Interrupt-Driven Servo Control - STM32F103C8T6
 * Built by Prajesh Vinod
 *
 * PA0 -> TIM2_CH1 -> Servo PWM
 * PA1 -> Status LED 
 * PA2 -> Button 1 -> EXTI2 - Internal Pull-up
 * PA3 -> Button 2 -> EXTI3 - Internal Pull-up
 */

#include "stm32f10x.h"

void TIM2_PWM_Init(void)
{
    RCC->APB2ENR |= (1 << 0);
    RCC->APB1ENR |= (1 << 0);

    // PA0: Alternate Function Push-Pull
    GPIOA->CRL &= ~(0xF << 0);
    GPIOA->CRL |=  (0xB << 0);

    // 1 MHz timer tick, 20 ms PWM period
    TIM2->PSC = 71;
    TIM2->ARR = 19999;

    // PWM Mode 1
    TIM2->CCMR1 &= ~(0x7 << 4);
    TIM2->CCMR1 |=  (0x6 << 4);
    TIM2->CCMR1 |=  (1 << 3);

    TIM2->CCER |= (1 << 0);
    TIM2->CCR1 = 1500;

    TIM2->CR1 |= (1 << 7);
    TIM2->CR1 |= (1 << 0);
}

void EXTI_Init(void)
{
    RCC->APB2ENR |= (1 << 0);

    // PA2 -> EXTI2, PA3 -> EXTI3
    AFIO->EXTICR[0] &= ~(0xF << 8);
    AFIO->EXTICR[0] &= ~(0xF << 12);

    // Rising and falling edge triggers
    EXTI->FTSR |= (1 << 2) | (1 << 3);
    EXTI->RTSR |= (1 << 2) | (1 << 3);

    EXTI->IMR |= (1 << 2) | (1 << 3);

    NVIC_EnableIRQ(EXTI2_IRQn);
    NVIC_EnableIRQ(EXTI3_IRQn);
}

void Update_Servo_State(void)
{
    uint8_t btn1 = (GPIOA->IDR & (1 << 2)) ? 1 : 0;
    uint8_t btn2 = (GPIOA->IDR & (1 << 3)) ? 1 : 0;

    if (btn1 == 0 && btn2 == 0)
    {
        TIM2->CCR1 = 1500;
        GPIOA->BSRR = (1 << 1);
    }
    else if (btn1 == 0)
    {
        TIM2->CCR1 = 500;
        GPIOA->BSRR = (1 << 1);
    }
    else if (btn2 == 0)
    {
        TIM2->CCR1 = 2500;
        GPIOA->BSRR = (1 << 1);
    }
    else
    {
        TIM2->CCR1 = 1500;
        GPIOA->BSRR = (1 << 17);
    }
}

void EXTI2_IRQHandler(void)
{
    if (EXTI->PR & (1 << 2))
    {
        Update_Servo_State();
        EXTI->PR = (1 << 2);
    }
}

void EXTI3_IRQHandler(void)
{
    if (EXTI->PR & (1 << 3))
    {
        Update_Servo_State();
        EXTI->PR = (1 << 3);
    }
}

int main(void)
{
    RCC->APB2ENR |= (1 << 2);

    // PA1: Status LED (Push-Pull 2MHz - 0x2)
    GPIOA->CRL &= ~(0xF << 4);// Clear 
    GPIOA->CRL |=  (0x2 << 4);// Set

    // PA2: Button 1 with pull-up
    GPIOA->CRL &= ~(0xF << 8);
    GPIOA->CRL |=  (0x8 << 8);
    GPIOA->ODR |= (1 << 2);

    // PA3: Button 2 with pull-up
    GPIOA->CRL &= ~(0xF << 12);
    GPIOA->CRL |=  (0x8 << 12);
    GPIOA->ODR |= (1 << 3);

    TIM2_PWM_Init();
    EXTI_Init();

    while (1)
    {
        __WFI();
    }
}

