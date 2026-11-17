/*
 * Bare-Metal I2C OLED Display - STM32F103C8T6
 * Built by Prajesh Vinod
 *
 * PB6 -> I2C1_SCL -> OLED SCL
 * PB7 -> I2C1_SDA -> OLED SDA
 *
 * I2C1 Communication -> SSD1306 OLED (0x3C)
 * Displays "BLUELINA ROX"
 */

#include "stm32f10x.h"
#include <stdint.h>

// 1. GPIO + I2C CLOCK CONFIGURATION

void I2C1_GPIO_Init(void)
{
    // Enable GPIOB clock (APB2)
    RCC->APB2ENR |= (1 << 3);

    // Enable I2C1 clock (APB1)
    RCC->APB1ENR |= (1 << 21);

    // PB6 = SCL, PB7 = SDA
    // Alternate Function Open Drain, 50 MHz
    GPIOB->CRL &= ~((0xF << 24) | (0xF << 28));
    GPIOB->CRL |=  ((0xF << 24) | (0xF << 28));
}


// 2. I2C1 CONFIGURATION

/* for standard mode 100khz
void I2C1_Init(void)

{

    // Disable I2C before configuration

    I2C1->CR1 &= ~(1 << 0);



    // APB1 clock = 36 MHz

    I2C1->CR2 = 36;



    // Standard mode: 100 kHz

    I2C1->CCR = 180;



    // Maximum rise time

    I2C1->TRISE = 37;



    // Enable I2C1

    I2C1->CR1 |= (1 << 0);

}
*/
//for fast mode - 400kHz
void I2C1_Init(void)
{
    // Disable I2C before configuration
    I2C1->CR1 &= ~(1 << 0);

    // APB1 clock = 36 MHz
    I2C1->CR2 = 36;

    // Fast mode: 400 kHz (Bit 15 = 1 for Fast Mode, CCR = 30)
    I2C1->CCR = (1 << 15) | 30;

    // Maximum rise time for Fast Mode (12)
    I2C1->TRISE = 12;

    // Enable I2C1
    I2C1->CR1 |= (1 << 0);
}

// 3. I2C COMMAND TRANSMISSION
void I2C1_WriteCommand(uint8_t command)
{
    // Generate START
    I2C1->CR1 |= (1 << 8);

    while (!(I2C1->SR1 & (1 << 0)));

    // Send OLED address + Write bit
    I2C1->DR = (0x3C << 1);

    while (!(I2C1->SR1 & (1 << 1)));

    // Clear ADDR flag
    volatile uint32_t temp = I2C1->SR1;
    temp = I2C1->SR2;
    (void)temp;

    // Send command control byte
    while (!(I2C1->SR1 & (1 << 7)));

    I2C1->DR = 0x00;

    // Send actual command
    while (!(I2C1->SR1 & (1 << 7)));

    I2C1->DR = command;

    // Wait for byte transfer finished
    while (!(I2C1->SR1 & (1 << 2)));

    // Generate STOP
    I2C1->CR1 |= (1 << 9);

    while (I2C1->CR1 & (1 << 9));
}

// 4. I2C DATA TRANSMISSION
void I2C1_WriteData(uint8_t data)
{
    // Generate START
    I2C1->CR1 |= (1 << 8);

    while (!(I2C1->SR1 & (1 << 0)));

    // Send OLED address + Write bit
    I2C1->DR = (0x3C << 1);

    while (!(I2C1->SR1 & (1 << 1)));

    // Clear ADDR flag
    volatile uint32_t temp = I2C1->SR1;
    temp = I2C1->SR2;
    (void)temp;

    // Send data control byte
    while (!(I2C1->SR1 & (1 << 7)));

    I2C1->DR = 0x40;

    // Send pixel data
    while (!(I2C1->SR1 & (1 << 7)));

    I2C1->DR = data;

    // Wait for completion
    while (!(I2C1->SR1 & (1 << 2)));

    // Generate STOP
    I2C1->CR1 |= (1 << 9);

    while (I2C1->CR1 & (1 << 9));
}

// 5. OLED INITIALIZATION
void OLED_Init(void)
{
    I2C1_WriteCommand(0xAE); // Display OFF

    I2C1_WriteCommand(0xD5);
    I2C1_WriteCommand(0x80);

    I2C1_WriteCommand(0xA8);
    I2C1_WriteCommand(0x3F);

    I2C1_WriteCommand(0xD3);
    I2C1_WriteCommand(0x00);

    I2C1_WriteCommand(0x40);

    I2C1_WriteCommand(0x8D);
    I2C1_WriteCommand(0x14);

    // Page addressing mode
    I2C1_WriteCommand(0x20);
    I2C1_WriteCommand(0x02);

    I2C1_WriteCommand(0xA1);
    I2C1_WriteCommand(0xC8);

    I2C1_WriteCommand(0xDA);
    I2C1_WriteCommand(0x12);

    I2C1_WriteCommand(0x81);
    I2C1_WriteCommand(0xCF);

    I2C1_WriteCommand(0xD9);
    I2C1_WriteCommand(0xF1);

    I2C1_WriteCommand(0xDB);
    I2C1_WriteCommand(0x40);

    I2C1_WriteCommand(0xA4);
    I2C1_WriteCommand(0xA6);

    I2C1_WriteCommand(0xAF); // Display ON
}

// 6. DIRECT PIXEL DATA: BLUELINA ROX
void OLED_PrintBlueLina(void)
{
    // Page 3, Column 0 (Centered vertically on 128x64 display)
    I2C1_WriteCommand(0xB3);
    I2C1_WriteCommand(0x00);
    I2C1_WriteCommand(0x10);

    // B
    I2C1_WriteData(0x7F);
    I2C1_WriteData(0x49);
    I2C1_WriteData(0x49);
    I2C1_WriteData(0x49);
    I2C1_WriteData(0x36);
    I2C1_WriteData(0x00);

    // L
    I2C1_WriteData(0x7F);
    I2C1_WriteData(0x40);
    I2C1_WriteData(0x40);
    I2C1_WriteData(0x40);
    I2C1_WriteData(0x40);
    I2C1_WriteData(0x00);

    // U
    I2C1_WriteData(0x3F);
    I2C1_WriteData(0x40);
    I2C1_WriteData(0x40);
    I2C1_WriteData(0x40);
    I2C1_WriteData(0x3F);
    I2C1_WriteData(0x00);

    // E
    I2C1_WriteData(0x7F);
    I2C1_WriteData(0x49);
    I2C1_WriteData(0x49);
    I2C1_WriteData(0x49);
    I2C1_WriteData(0x41);
    I2C1_WriteData(0x00);

    // L
    I2C1_WriteData(0x7F);
    I2C1_WriteData(0x40);
    I2C1_WriteData(0x40);
    I2C1_WriteData(0x40);
    I2C1_WriteData(0x40);
    I2C1_WriteData(0x00);

    // I
    I2C1_WriteData(0x00);
    I2C1_WriteData(0x41);
    I2C1_WriteData(0x7F);
    I2C1_WriteData(0x41);
    I2C1_WriteData(0x00);
    I2C1_WriteData(0x00);

    // N
    I2C1_WriteData(0x7F);
    I2C1_WriteData(0x06);
    I2C1_WriteData(0x18);
    I2C1_WriteData(0x60);
    I2C1_WriteData(0x7F);
    I2C1_WriteData(0x00);

    // A
    I2C1_WriteData(0x7E);
    I2C1_WriteData(0x11);
    I2C1_WriteData(0x11);
    I2C1_WriteData(0x11);
    I2C1_WriteData(0x7E);
    I2C1_WriteData(0x00);

    // Space (Fixed to 3 columns for clean spacing)
    I2C1_WriteData(0x00);
    I2C1_WriteData(0x00);
    I2C1_WriteData(0x00);

    // R
    I2C1_WriteData(0x7F);
    I2C1_WriteData(0x09);
    I2C1_WriteData(0x19);
    I2C1_WriteData(0x29);
    I2C1_WriteData(0x46);
    I2C1_WriteData(0x00);

    // O
    I2C1_WriteData(0x3E);
    I2C1_WriteData(0x41);
    I2C1_WriteData(0x41);
    I2C1_WriteData(0x41);
    I2C1_WriteData(0x3E);
    I2C1_WriteData(0x00);

    // X
    I2C1_WriteData(0x63);
    I2C1_WriteData(0x14);
    I2C1_WriteData(0x08);
    I2C1_WriteData(0x14);
    I2C1_WriteData(0x63);
    I2C1_WriteData(0x00);
}

// OLED CLEAR FUNCTION
void OLED_Clear(void)
{
    for (uint8_t page = 0; page < 8; page++)
    {
        // Select page (0 to 7)
        I2C1_WriteCommand(0xB0 + page);

        // Set starting column to 0
        I2C1_WriteCommand(0x00);
        I2C1_WriteCommand(0x10);

        // Clear all 128 columns in this page
        for (uint8_t col = 0; col < 128; col++)
        {
            I2C1_WriteData(0x00);
        }}}


// 7. MAIN

int main(void)
{
    I2C1_GPIO_Init();
    I2C1_Init();

    for (volatile uint32_t i = 0; i < 500000; i++);

    OLED_Init();
	        OLED_Clear();
        OLED_PrintBlueLina();

    while (1)
    {
    }
}
