
#ifndef LAMPS595_H_
#define LAMPS595_H_

#include <stdint.h>

/* Lamp Bit Definitions (74HC595 Shift Register Bit Mapping) */
#define LAMP_LOW_FUEL       (1U << 0)  /* Q0: Low fuel (Amber)       */
#define LAMP_OIL_PRESS      (1U << 1)  /* Q1: Oil pressure (Red)     */
#define LAMP_BATTERY        (1U << 2)  /* Q2: Battery (Red)          */
#define LAMP_COOLANT_TEMP   (1U << 3)  /* Q3: Coolant temp (Red)     */
#define LAMP_CHECK_ENGINE   (1U << 4)  /* Q4: Check engine (Amber)   */
#define LAMP_LEFT_TURN      (1U << 5)  /* Q5: Left turn (Green)      */
#define LAMP_RIGHT_TURN     (1U << 6)  /* Q6: Right turn (Green)     */
#define LAMP_HIGH_BEAM      (1U << 7)  /* Q7: High beam (Blue)       */

#define LAMP_ALL_MASK       (0xFFU)

/**
 * @brief Initialize the 74HC595 lamp driver hardware pins and initial state.
 * Clears shift register outputs.
 */
void Lamps595_Init(void);

/**
 * @brief Set specific lamp bit(s) in the local buffer.
 * @param lamp_mask Bitmask of lamps to set.
 */
void Lamps595_SetLamp(uint8_t lamp_mask);

/**
 * @brief Clear specific lamp bit(s) in the local buffer.
 * @param lamp_mask Bitmask of lamps to clear.
 */
void Lamps595_ClearLamp(uint8_t lamp_mask);

/**
 * @brief Directly set the entire output buffer state.
 * @param byte Raw byte representing the active lamps.
 */
void Lamps595_WriteOutputs(uint8_t byte);

/**
 * @brief Get current lamp buffer state.
 * @return Current output byte buffer.
 */
uint8_t Lamps595_GetOutputs(void);

/**
 * @brief Refresh/shift out the current lamp state byte to the 74HC595 via SPI.
 * Latches output via PC2 strobe pulse. Safe against bus conflict.
 */
void Lamps595_Update(void);

/**
 * @brief Turn all cluster lamps ON (for 3 s bulb-check sequence).
 */
void Lamps595_BulbCheck(void);

#endif /* LAMPS595_H_ */