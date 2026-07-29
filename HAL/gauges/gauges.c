#include "gauges.h"

#include <stdint.h>

#define GAU_SAMPLE_COUNT 8u
#define GAU_MEDIAN_COUNT 3u
#define GAU_INVALID_TICKS 10u
#define GAU_MAX_RAW 1023u
#define GAU_FUEL_SCALE 100u
#define GAU_COOLANT_SCALE 170u
#define GAU_BATTERY_SCALE 16000u
#define GAU_OIL_SCALE 100u

typedef enum
{
    GAU_CH_FUEL = 0u,
    GAU_CH_COOLANT = 1u,
    GAU_CH_BATTERY = 2u,
    GAU_CH_OIL = 3u,
    GAU_CH_COUNT = 4u
} GaugesChannel_t;

typedef struct
{
    uint16_t raw;
    uint16_t filtered;
    uint16_t samples[GAU_SAMPLE_COUNT];
    uint8_t  sampleIndex;
    uint8_t  sampleCount;
    uint16_t invalidTicks;
    uint8_t  valid;
} GaugeState_t;

static GaugeState_t s_gauge[GAU_CH_COUNT];

static uint16_t GAU_Median3(uint16_t a, uint16_t b, uint16_t c)
{
    uint16_t x = a;
    uint16_t y = b;
    uint16_t z = c;

    if (x > y)
    {
        uint16_t t = x;
        x = y;
        y = t;
    }

    if (y > z)
    {
        uint16_t t = y;
        y = z;
        z = t;
    }

    if (x > y)
    {
        uint16_t t = x;
        x = y;
        y = t;
    }

    return y;
}

static uint16_t GAU_MovingAverage(uint16_t newValue, GaugeState_t *state)
{
    uint32_t sum;
    uint8_t i;

    state->samples[state->sampleIndex] = newValue;
    state->sampleIndex++;
    if (state->sampleIndex >= GAU_SAMPLE_COUNT)
    {
        state->sampleIndex = 0u;
    }

    if (state->sampleCount < GAU_SAMPLE_COUNT)
    {
        state->sampleCount++;
    }

    sum = 0u;
    for (i = 0u; i < state->sampleCount; i++)
    {
        sum += state->samples[i];
    }

    return (uint16_t)(sum / state->sampleCount);
}

static void GAU_UpdateState(GaugesChannel_t channel, uint16_t raw)
{
    GaugeState_t *state = &s_gauge[channel];
    uint16_t filtered;

    state->raw = raw;

    if ((channel == GAU_CH_FUEL) || (channel == GAU_CH_COOLANT))
    {
        filtered = GAU_MovingAverage(raw, state);
    }
    else
    {
        uint16_t a;
        uint16_t b;
        uint16_t c;

        if (state->sampleCount < GAU_MEDIAN_COUNT)
        {
            state->samples[state->sampleCount] = raw;
            state->sampleCount++;
            filtered = raw;
        }
        else
        {
            for (uint8_t i = GAU_MEDIAN_COUNT - 1u; i > 0u; i--)
            {
                state->samples[i] = state->samples[i - 1u];
            }
            state->samples[0u] = raw;
            a = state->samples[0u];
            b = state->samples[1u];
            c = state->samples[2u];
            filtered = GAU_Median3(a, b, c);
        }
    }

    state->filtered = filtered;

    if ((raw == 0u) || (raw == GAU_MAX_RAW))
    {
        if (state->invalidTicks < GAU_INVALID_TICKS)
        {
            state->invalidTicks++;
        }
        else
        {
            state->valid = 0u;
        }
    }
    else
    {
        state->invalidTicks = 0u;
        state->valid = 1u;
    }
}

void GAU_Init(void)
{
    uint8_t i;

    for (i = 0u; i < GAU_CH_COUNT; i++)
    {
        s_gauge[i].raw = 0u;
        s_gauge[i].filtered = 0u;
        s_gauge[i].sampleIndex = 0u;
        s_gauge[i].sampleCount = 0u;
        s_gauge[i].invalidTicks = 0u;
        s_gauge[i].valid = 1u;
    }
}

void GAU_Update(void)
{
    /* TODO: integrate with the ADC driver once available. */
}

uint8_t GAU_Fuel(void)
{
    uint32_t scaled;
    uint8_t fuelPct;

    if (s_gauge[GAU_CH_FUEL].valid == 0u)
    {
        return 0u;
    }

    scaled = ((uint32_t)s_gauge[GAU_CH_FUEL].filtered * GAU_FUEL_SCALE) / GAU_MAX_RAW;
    fuelPct = (uint8_t)scaled;

    if (fuelPct > 100u)
    {
        fuelPct = 100u;
    }

    return fuelPct;
}

int16_t GAU_CoolantC(void)
{
    uint32_t scaled;
    int16_t coolantC;

    if (s_gauge[GAU_CH_COOLANT].valid == 0u)
    {
        return 0;
    }

    scaled = ((uint32_t)s_gauge[GAU_CH_COOLANT].filtered * GAU_COOLANT_SCALE) / GAU_MAX_RAW;
    coolantC = (int16_t)(scaled - 40u);

    return coolantC;
}

uint16_t GAU_BattmV(void)
{
    uint32_t scaled;
    uint16_t battmV;

    if (s_gauge[GAU_CH_BATTERY].valid == 0u)
    {
        return 0u;
    }

    scaled = ((uint32_t)s_gauge[GAU_CH_BATTERY].filtered * GAU_BATTERY_SCALE) / GAU_MAX_RAW;
    battmV = (uint16_t)scaled;

    if (battmV > 16000u)
    {
        battmV = 16000u;
    }

    return battmV;
}

uint8_t GAU_OilBarX10(void)
{
    uint32_t scaled;
    uint8_t oilBarX10;

    if (s_gauge[GAU_CH_OIL].valid == 0u)
    {
        return 0u;
    }

    scaled = ((uint32_t)s_gauge[GAU_CH_OIL].filtered * GAU_OIL_SCALE) / GAU_MAX_RAW;
    oilBarX10 = (uint8_t)scaled;

    if (oilBarX10 > 100u)
    {
        oilBarX10 = 100u;
    }

    return oilBarX10;
}

uint8_t GAU_IsValid(uint8_t channel)
{
    if (channel >= GAU_CH_COUNT)
    {
        return 0u;
    }

    return s_gauge[channel].valid;
}
