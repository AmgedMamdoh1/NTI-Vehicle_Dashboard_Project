#include "speedo.h"

#include <stdint.h>

#define SPD_TICK_US      8u
#define SPD_NUM          1800000u
#define SPD_STALL_TICKS 100u
#define SPD_WRAP_TICKS   65536u

typedef struct
{
    volatile uint16_t lastIcr;
    volatile uint16_t ovfCount;
    volatile uint32_t deltaTicks;
    volatile uint8_t  fresh;
    uint16_t          stallTicks;
} Capture_t;

static Capture_t s_capture;
static uint16_t s_speedKmh;
static uint8_t s_speedValid;

static uint32_t SPD_PeriodUs(uint32_t deltaTicks)
{
    return deltaTicks * SPD_TICK_US;
}

static uint16_t SPD_ConvertKmh(uint32_t periodUs)
{
    if (periodUs == 0u)
    {
        return 0u;
    }

    return (uint16_t)(SPD_NUM / periodUs);
}

void SPD_Init(void)
{
    s_capture.lastIcr = 0u;
    s_capture.ovfCount = 0u;
    s_capture.deltaTicks = 0u;
    s_capture.fresh = 0u;
    s_capture.stallTicks = 0u;
    s_speedKmh = 0u;
    s_speedValid = 0u;
}

void SPD_Update(void)
{
    uint32_t deltaTicks;
    uint32_t periodUs;
    uint16_t speedKmh;

    if (s_capture.fresh == 0u)
    {
        if (s_capture.stallTicks < SPD_STALL_TICKS)
        {
            s_capture.stallTicks++;
        }
        else
        {
            s_speedKmh = 0u;
            s_speedValid = 0u;
        }
        return;
    }

    deltaTicks = s_capture.deltaTicks;
    s_capture.fresh = 0u;

    periodUs = SPD_PeriodUs(deltaTicks);
    speedKmh = SPD_ConvertKmh(periodUs);

    if (speedKmh > 250u)
    {
        speedKmh = 0u;
        s_speedValid = 0u;
    }
    else
    {
        s_speedValid = 1u;
    }

    s_capture.stallTicks = 0u;
    s_speedKmh = speedKmh;
}

void SPD_OnCapture(uint16_t captureTicks)
{
    uint32_t deltaTicks;

    if (s_capture.lastIcr == 0u)
    {
        s_capture.lastIcr = captureTicks;
        return;
    }

    deltaTicks = (uint32_t)captureTicks - (uint32_t)s_capture.lastIcr;
    if (captureTicks < s_capture.lastIcr)
    {
        deltaTicks += SPD_WRAP_TICKS;
    }

    deltaTicks += ((uint32_t)s_capture.ovfCount) * SPD_WRAP_TICKS;

    s_capture.deltaTicks = deltaTicks;
    s_capture.fresh = 1u;
    s_capture.lastIcr = captureTicks;
    s_capture.ovfCount = 0u;
}

void SPD_OnOverflow(void)
{
    s_capture.ovfCount++;
}

uint16_t SPD_GetKmh(void)
{
    return s_speedKmh;
}

uint8_t SPD_IsValid(void)
{
    return s_speedValid;
}
