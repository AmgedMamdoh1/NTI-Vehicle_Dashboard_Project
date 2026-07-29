
#include "lcd_i2c.h"

/* PCF8574 I2C Slave Address (Default 0x27 shifted left by 1 for write mode: 0x4E) */
#define PCF8574_I2C_ADDR        0x4EU

/* PCF8574 Pin Mapping to HD44780 */
#define LCD_BIT_RS              (1U << 0)  /* Register Select: 0=Command, 1=Data */
#define LCD_BIT_RW              (1U << 1)  /* Read/Write: 0=Write                 */
#define LCD_BIT_EN              (1U << 2)  /* Enable Pulse                        */
#define LCD_BIT_BACKLIGHT       (1U << 3)  /* Backlight Control LED Pin           */

/* Private Module State */
static uint8_t s_backlightMask = LCD_BIT_BACKLIGHT;

/* TODO: Remove external MCAL declarations once MCAL i2c.h exists */
extern void I2C_Init(uint32_t clock_hz);
extern uint8_t I2C_Start(void);
extern void I2C_Stop(void);
extern uint8_t I2C_Write(uint8_t data);
extern void _delay_ms(double ms);
extern void _delay_us(double us);

/* Private helper declarations */
static void Lcd_SendNibble(uint8_t nibble, uint8_t rs);
static void Lcd_SendByte(uint8_t byte, uint8_t rs);
static void Lcd_WriteCmd(uint8_t cmd);

static void Lcd_SendNibble(uint8_t nibble, uint8_t rs)
{
    uint8_t data = (nibble & 0xF0U) | s_backlightMask;
    if (rs != 0U)
    {
        data |= LCD_BIT_RS;
    }

    /* Perform I2C Transmission to PCF8574 */
    /* TODO: Integrate with MCAL I2C API */
    (void)I2C_Start();
    (void)I2C_Write(PCF8574_I2C_ADDR);

    /* Pulse EN High */
    (void)I2C_Write(data | LCD_BIT_EN);
    /* Enable pulse width >= 450 ns */
    /* _delay_us(1.0); */

    /* Pulse EN Low to latch data into HD44780 */
    (void)I2C_Write(data & (uint8_t)(~LCD_BIT_EN));
    /* Data execution / settling time */
    /* _delay_us(50.0); */

    I2C_Stop();
}

static void Lcd_SendByte(uint8_t byte, uint8_t rs)
{
    Lcd_SendNibble(byte & 0xF0U, rs);
    Lcd_SendNibble((uint8_t)(byte << 4U), rs);
}

static void Lcd_WriteCmd(uint8_t cmd)
{
    Lcd_SendByte(cmd, 0U);
}

void Lcd_Init(void)
{
    /* TODO: Initialize I2C hardware at 100 kHz */
    /* I2C_Init(100000UL); */

    /* Wait > 40 ms after VCC reaches 2.7V */
    /* _delay_ms(50.0); */

    s_backlightMask = LCD_BIT_BACKLIGHT;

    /* Initialization sequence for HD44780 in 4-bit mode */
    Lcd_SendNibble(0x30U, 0U);
    /* _delay_ms(4.5); */

    Lcd_SendNibble(0x30U, 0U);
    /* _delay_us(150.0); */

    Lcd_SendNibble(0x30U, 0U);
    /* _delay_us(150.0); */

    /* Set to 4-bit interface mode */
    Lcd_SendNibble(0x20U, 0U);
    /* _delay_ms(2.0); */

    /* Function set: 4-bit, 2 lines, 5x8 font */
    Lcd_WriteCmd(0x28U);
    /* _delay_ms(2.0); */

    /* Display control: Display ON, Cursor OFF, Blink OFF */
    Lcd_WriteCmd(0x0CU);
    /* _delay_ms(2.0); */

    /* Clear display */
    Lcd_Clear();

    /* Entry mode set: Increment cursor, no shift */
    Lcd_WriteCmd(0x06U);
    /* _delay_ms(2.0); */
}

void Lcd_Clear(void)
{
    Lcd_WriteCmd(0x01U);
    /* Clear display requires ~1.53 ms execution time */
    /* _delay_ms(2.0); */
}

void Lcd_SetCursor(uint8_t row, uint8_t col)
{
    uint8_t address;

    if (col >= LCD_COLS)
    {
        col = LCD_COLS - 1U;
    }

    if (row == 0U)
    {
        address = 0x80U + col;
    }
    else if (row == 1U)
    {
        address = 0xC0U + col;
    }
    else
    {
        address = 0x80U + col;
    }

    Lcd_WriteCmd(address);
}

void Lcd_WriteChar(char chr)
{
    Lcd_SendByte((uint8_t)chr, 1U);
}

void Lcd_WriteString(const char *str)
{
    if (str == ((void *)0))
    {
        return;
    }

    while (*str != '\0')
    {
        Lcd_WriteChar(*str);
        str++;
    }
}

void Lcd_Backlight(uint8_t state)
{
    if (state != 0U)
    {
        s_backlightMask = LCD_BIT_BACKLIGHT;
    }
    else
    {
        s_backlightMask = 0x00U;
    }

    /* Refresh current state to apply backlight toggle immediately */
    Lcd_SendNibble(0x00U, 0U);
}

void Lcd_CreateChar(uint8_t location, const uint8_t charmap[8])
{
    uint8_t i;

    if (location >= 8U || charmap == ((void *)0))
    {
        return;
    }

    /* Set CGRAM address (0x40 + location * 8) */
    Lcd_WriteCmd((uint8_t)(0x40U | (location << 3U)));

    for (i = 0U; i < 8U; i++)
    {
        Lcd_SendByte(charmap[i], 1U);
    }
}