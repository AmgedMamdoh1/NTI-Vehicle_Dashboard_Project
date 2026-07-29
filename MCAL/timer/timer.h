#ifndef TIMER_H_
#define TIMER_H_

#include <stdint.h>

/* Callback Function Pointer Type */
typedef void (*Timer_Callback_t)(void);

/**
 * @brief Initialize Timer0 for System Tick generation (CTC Mode, 1 ms tick rate).
 * F_CPU = 8 MHz, Prescaler = 64, OCR0 = 124 -> (8000000 / (64 * (124 + 1))) = 1000 Hz (1 ms).
 */
void Timer_Init(void);

/**
 * @brief Register callback function for Timer0 Compare Match ISR (System Tick).
 * @param callback Pointer to function to be executed every 1 ms.
 */
void Timer_SetCallback(Timer_Callback_t callback);

/**
 * @brief Enable Timer0 Compare Match Interrupt.
 */
void Timer_EnableInterrupt(void);

/**
 * @brief Disable Timer0 Compare Match Interrupt.
 */
void Timer_DisableInterrupt(void);

/**
 * @brief Get total elapsed system tick count since initialization.
 * @return 32-bit tick count in milliseconds.
 */
uint32_t Timer_GetTicks(void);

/**
 * @brief Start Timer0 clock.
 */
void Timer_Start(void);

/**
 * @brief Stop Timer0 clock.
 */
void Timer_Stop(void);

#endif /* TIMER_H_ */