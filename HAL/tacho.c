#include "tacho.h"

#include <stdint.h>

#define TAC_RPM_FACTOR 120u
#define TAC_WINDOW_TICKS 250u
#define TAC_MAX_RPM 8000u

typedef struct
{
    uint16_t pulseCount;
    uint16_t rpm;
    uint8_t  valid;
    uint16_t windowTicks;
} TacState_t;

static TacState_t s_tacho;

void TAC_Init(void)
{
    s_tacho.pulseCount = 0u;
    s_tacho.rpm = 0u;
    s_tacho.valid = 0u;
    s_tacho.windowTicks = 0u;
}

void TAC_OnPulse(void)
{
    if (s_tacho.pulseCount < 0xFFFFu)
    {
        s_tacho.pulseCount++;
    }
}

void TAC_Update250ms(void)
{
    uint16_t rpm;

    rpm = (uint16_t)(s_tacho.pulseCount * TAC_RPM_FACTOR);

    if (rpm > TAC_MAX_RPM)
    {
        rpm = TAC_MAX_RPM;
    }

    s_tacho.rpm = rpm;
    s_tacho.valid = 1u;

    s_tacho.pulseCount = 0u;
}

uint16_t TAC_GetRpm(void)
{
    return s_tacho.rpm;
}

uint8_t TAC_IsValid(void)
{
    return s_tacho.valid;
}
