#include "chime.h"

/* Hardware Pin Mapping per README Pin Map (Section 7) */
#define CHIME_PORT      PORT_D   /* PD7: Active-high piezobuzzer / OC2 */
#define CHIME_PIN       PIN_7

/* Private Module State */
static Chime_Pattern_t s_currentPattern = CHIME_PATTERN_OFF;
static uint16_t s_stepTimer = 0U;   /* Step duration counter in 10 ms ticks */
static uint8_t s_patternStep = 0U;  /* Current step index within pattern   */

/* External MCAL / GPIO declarations */
extern void DIO_SetPinDirection(uint8_t port, uint8_t pin, uint8_t direction);
extern void DIO_SetPinLevel(uint8_t port, uint8_t pin, uint8_t level);

/* Helper to actuate buzzer output pin */
static void Chime_SetHardwareState(uint8_t state)
{
    /* TODO: Set PD7 pin level via DIO driver */
    /* DIO_SetPinLevel(CHIME_PORT, CHIME_PIN, state ? HIGH : LOW); */
    (void)state;
}

void Chime_Init(void)
{
    /* TODO: Set PD7 as output */
    /* DIO_SetPinDirection(CHIME_PORT, CHIME_PIN, OUTPUT); */
    Chime_Stop();
}

void Chime_Play(Chime_Pattern_t pattern)
{
    if (pattern >= CHIME_PATTERN_CONTINUOUS + 1)
    {
        return;
    }

    s_currentPattern = pattern;
    s_patternStep = 0U;
    s_stepTimer = 0U;

    if (pattern == CHIME_PATTERN_OFF)
    {
        Chime_SetHardwareState(0U);
    }
    else
    {
        /* Immediately turn buzzer ON for non-OFF patterns */
        Chime_SetHardwareState(1U);
    }
}

void Chime_Stop(void)
{
    s_currentPattern = CHIME_PATTERN_OFF;
    s_patternStep = 0U;
    s_stepTimer = 0U;
    Chime_SetHardwareState(0U);
}

void Chime_Update(void)
{
    if (s_currentPattern == CHIME_PATTERN_OFF)
    {
        return;
    }

    s_stepTimer++;

    switch (s_currentPattern)
    {
        case CHIME_PATTERN_SINGLE_SHORT:
            /* 100 ms (10 ticks @ 10 ms update) */
            if (s_stepTimer >= 10U)
            {
                Chime_Stop();
            }
            break;

        case CHIME_PATTERN_SINGLE_LONG:
            /* 500 ms (50 ticks @ 10 ms update) */
            if (s_stepTimer >= 50U)
            {
                Chime_Stop();
            }
            break;

        case CHIME_PATTERN_DOUBLE_BEEP:
            /* Step 0: ON 100 ms -> Step 1: OFF 100 ms -> Step 2: ON 100 ms -> STOP */
            if (s_patternStep == 0U && s_stepTimer >= 10U)
            {
                s_patternStep = 1U;
                s_stepTimer = 0U;
                Chime_SetHardwareState(0U);
            }
            else if (s_patternStep == 1U && s_stepTimer >= 10U)
            {
                s_patternStep = 2U;
                s_stepTimer = 0U;
                Chime_SetHardwareState(1U);
            }
            else if (s_patternStep == 2U && s_stepTimer >= 10U)
            {
                Chime_Stop();
            }
            break;

        case CHIME_PATTERN_CONTINUOUS:
            /* Remains ON indefinitely until Chime_Stop() is explicitly called */
            break;

        default:
            Chime_Stop();
            break;
    }
}

uint8_t Chime_IsBusy(void)
{
    return (s_currentPattern != CHIME_PATTERN_OFF) ? 1U : 0U;
}