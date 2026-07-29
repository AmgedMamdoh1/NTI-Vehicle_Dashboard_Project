#include "timer.h"
#include <avr/io.h>
#include <avr/interrupt.h>

/* Private module variables */
static volatile uint32_t s_systemTicks = 0U;
static Timer_Callback_t s_tickCallback = ((void *)0);

void Timer_Init(void)
{
    /* Set Timer0 to CTC (Clear Timer on Compare Match) Mode: WGM01 = 1, WGM00 = 0 */
    TCCR0 = (1U << WGM01);

    /* Set Output Compare Register for 1 ms tick @ 8 MHz F_CPU with Prescaler 64
     * Target frequency = 1000 Hz
     * OCR0 = (8,000,000 / (64 * 1000)) - 1 = 124
     */
    OCR0 = 124U;

    /* Reset Timer Counter Register */
    TCNT0 = 0U;

    /* Reset state variables */
    s_systemTicks = 0U;

    /* Enable Output Compare Match Interrupt (OCIE0) */
    TIMSK |= (1U << OCIE0);

    /* Start Timer0 with Prescaler 64 (CS01 = 1, CS00 = 1) */
    Timer_Start();
}

void Timer_Start(void)
{
    /* Set Prescaler 64: CS02=0, CS01=1, CS00=1 */
    TCCR0 = (uint8_t)((TCCR0 & 0xF8U) | (1U << CS01) | (1U << CS00));
}

void Timer_Stop(void)
{
    /* Clear clock source bits (CS02..CS00 = 0) */
    TCCR0 &= (uint8_t)(~((1U << CS02) | (1U << CS01) | (1U << CS00)));
}

void Timer_SetCallback(Timer_Callback_t callback)
{
    s_tickCallback = callback;
}

void Timer_EnableInterrupt(void)
{
    TIMSK |= (1U << OCIE0);
}

void Timer_DisableInterrupt(void)
{
    TIMSK &= (uint8_t)(~(1U << OCIE0));
}

uint32_t Timer_GetTicks(void)
{
    uint32_t ticks;

    /* Atomic read of 32-bit tick counter */
    uint8_t sreg = SREG;
    cli();
    ticks = s_systemTicks;
    SREG = sreg;

    return ticks;
}

/* Timer0 Compare Match Interrupt Service Routine */
ISR(TIMER0_COMP_vect)
{
    s_systemTicks++;

    if (s_tickCallback != ((void *)0))
    {
        s_tickCallback();
    }
}