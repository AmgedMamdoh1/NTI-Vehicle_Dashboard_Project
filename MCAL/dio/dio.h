#ifndef DIO_H_
#define DIO_H_

#include <stdint.h>

/* Port Identifier Definitions */
#define PORT_A  0U
#define PORT_B  1U
#define PORT_C  2U
#define PORT_D  3U

/* Pin Direction Definitions */
#define INPUT   0U
#define OUTPUT  1U

/* Pin State / Level Definitions */
#define LOW     0U
#define HIGH    1U

/**
 * @brief Initialize DIO hardware module (registers cleared/default setup).
 */
void DIO_Init(void);

/**
 * @brief Configure direction for a specific pin.
 * @param port Port ID (PORT_A, PORT_B, PORT_C, PORT_D).
 * @param pin Pin number (0 to 7).
 * @param direction Direction (INPUT or OUTPUT).
 */
void DIO_SetPinDirection(uint8_t port, uint8_t pin, uint8_t direction);

/**
 * @brief Set output logic level for a specific pin.
 * @param port Port ID (PORT_A, PORT_B, PORT_C, PORT_D).
 * @param pin Pin number (0 to 7).
 * @param level State level (LOW or HIGH).
 */
void DIO_SetPinLevel(uint8_t port, uint8_t pin, uint8_t level);

/**
 * @brief Read logic level from a specific pin.
 * @param port Port ID (PORT_A, PORT_B, PORT_C, PORT_D).
 * @param pin Pin number (0 to 7).
 * @return Current logic level (LOW or HIGH). Returns LOW on invalid parameters.
 */
uint8_t DIO_GetPinLevel(uint8_t port, uint8_t pin);

/**
 * @brief Toggle output level of a specific pin.
 * @param port Port ID (PORT_A, PORT_B, PORT_C, PORT_D).
 * @param pin Pin number (0 to 7).
 */
void DIO_TogglePin(uint8_t port, uint8_t pin);

/**
 * @brief Enable internal pull-up resistor for an input pin.
 * @param port Port ID (PORT_A, PORT_B, PORT_C, PORT_D).
 * @param pin Pin number (0 to 7).
 */
void DIO_EnablePullUp(uint8_t port, uint8_t pin);

/**
 * @brief Write full byte value to a complete port output register.
 * @param port Port ID (PORT_A, PORT_B, PORT_C, PORT_D).
 * @param value Byte value to write to PORTx register.
 */
void DIO_WritePort(uint8_t port, uint8_t value);

/**
 * @brief Read full byte value from a complete port input register (PINx).
 * @param port Port ID (PORT_A, PORT_B, PORT_C, PORT_D).
 * @return Raw 8-bit port input register value.
 */
uint8_t DIO_ReadPort(uint8_t port);

#endif /* DIO_H_ */