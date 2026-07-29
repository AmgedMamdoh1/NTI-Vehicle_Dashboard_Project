#ifndef LCD_I2C_H_
#define LCD_I2C_H_

#include <stdint.h>

/* Display Dimensions */
#define LCD_ROWS        2U
#define LCD_COLS        16U

/**
 * @brief Initialize HD44780 LCD in 4-bit mode via PCF8574 over I2C.
 * Sets up 2-line mode, 5x8 font, display ON, cursor OFF, blink OFF.
 */
void Lcd_Init(void);

/**
 * @brief Clear LCD screen and move cursor to (0, 0).
 */
void Lcd_Clear(void);

/**
 * @brief Set cursor position.
 * @param row Row index (0 or 1).
 * @param col Column index (0 to 15).
 */
void Lcd_SetCursor(uint8_t row, uint8_t col);

/**
 * @brief Write a single ASCII character to the current cursor position.
 * @param chr ASCII character to display.
 */
void Lcd_WriteChar(char chr);

/**
 * @brief Write a null-terminated string to the display.
 * @param str Pointer to string.
 */
void Lcd_WriteString(const char *str);

/**
 * @brief Control LCD backlight state (PC7 / PCF8574 Backlight Bit).
 * @param state 1 for ON, 0 for OFF.
 */
void Lcd_Backlight(uint8_t state);

/**
 * @brief Create a custom character pattern in CGRAM.
 * @param location Custom character index (0 to 7).
 * @param charmap Array of 8 bytes defining 5x8 pixel matrix.
 */
void Lcd_CreateChar(uint8_t location, const uint8_t charmap[8]);

#endif /* LCD_I2C_H_ */