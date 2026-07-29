#include <stddef.h>

#include "warnings.h"

#define WRN_OIL_DELAY_TICKS      200u
#define WRN_BATT_DELAY_TICKS     500u
#define WRN_COOLANT_DELAY_TICKS  300u

static uint16_t s_delayTicks[WARN_HANDBRAKE + 1u];
static uint16_t s_latchedMask;
static uint8_t s_fuelActive;
static uint8_t s_overspeedActive;
static ClusterState_t s_prevState;

static uint16_t WarnBit(Warn_t warning)
{
    return (uint16_t)(1u << (uint16_t)warning);
}

static uint8_t IsLatchingWarning(Warn_t warning)
{
    return (warning == WARN_OIL) || (warning == WARN_COOLANT);
}

static uint8_t UpdateDelay(Warn_t warning, uint8_t condition, uint16_t threshold)
{
    uint16_t index = (uint16_t)warning;

    if (condition != 0u)
    {
        if (s_delayTicks[index] < threshold)
        {
            s_delayTicks[index]++;
        }

        return (s_delayTicks[index] >= threshold) ? 1u : 0u;
    }

    s_delayTicks[index] = 0u;
    return 0u;
}

static void ClearPendingState(void)
{
    uint16_t index;

    s_latchedMask = 0u;
    s_fuelActive = 0u;
    s_overspeedActive = 0u;

    for (index = 0u; index <= (uint16_t)WARN_HANDBRAKE; index++)
    {
        s_delayTicks[index] = 0u;
    }
}

void WRN_Init(void)
{
    ClearPendingState();
    s_prevState = CS_OFF;
}

void WRN_Update(CarData_t *data)
{
    uint16_t mask = 0u;
    uint8_t active;
    uint8_t fuelActive;
    uint8_t overspeedActive;

    if (data == NULL)
    {
        return;
    }

    if (s_prevState != CS_OFF && data->state == CS_OFF)
    {
        ClearPendingState();
    }

    if (data->state == CS_OFF)
    {
        data->warnMask = 0u;
        s_prevState = data->state;
        return;
    }

    fuelActive = s_fuelActive;
    overspeedActive = s_overspeedActive;

    /* Oil pressure warning: delayed while engine is running. */
    active = 0u;
    if (data->engineRun != 0u && data->oilBarX10 < 10u)
    {
        active = UpdateDelay(WARN_OIL, 1u, WRN_OIL_DELAY_TICKS);
    }
    else
    {
        UpdateDelay(WARN_OIL, 0u, WRN_OIL_DELAY_TICKS);
    }

    if (active != 0u)
    {
        mask |= WarnBit(WARN_OIL);
        if (IsLatchingWarning(WARN_OIL) != 0u)
        {
            s_latchedMask |= WarnBit(WARN_OIL);
        }
    }
    else if ((s_latchedMask & WarnBit(WARN_OIL)) != 0u)
    {
        mask |= WarnBit(WARN_OIL);
    }

    /* Battery warning: delayed low/high thresholds while engine is running. */
    active = 0u;
    if (data->engineRun != 0u)
    {
        if (data->battmV < 12000u || data->battmV > 15000u)
        {
            active = UpdateDelay(WARN_BATT, 1u, WRN_BATT_DELAY_TICKS);
        }
        else
        {
            UpdateDelay(WARN_BATT, 0u, WRN_BATT_DELAY_TICKS);
        }
    }
    else
    {
        UpdateDelay(WARN_BATT, 0u, WRN_BATT_DELAY_TICKS);
    }

    if (active != 0u)
    {
        mask |= WarnBit(WARN_BATT);
    }

    /* Coolant warning: delayed while the engine is running. */
    active = 0u;
    if (data->engineRun != 0u && data->coolantC > 110)
    {
        active = UpdateDelay(WARN_COOLANT, 1u, WRN_COOLANT_DELAY_TICKS);
    }
    else
    {
        UpdateDelay(WARN_COOLANT, 0u, WRN_COOLANT_DELAY_TICKS);
    }

    if (active != 0u)
    {
        mask |= WarnBit(WARN_COOLANT);
        if (IsLatchingWarning(WARN_COOLANT) != 0u)
        {
            s_latchedMask |= WarnBit(WARN_COOLANT);
        }
    }
    else if ((s_latchedMask & WarnBit(WARN_COOLANT)) != 0u)
    {
        mask |= WarnBit(WARN_COOLANT);
    }

    /* Check warning for implausible values. */
    if (data->speedKmh > 250u || data->rpm > 5500u)
    {
        mask |= WarnBit(WARN_CHECK);
    }

    /* Fuel warning uses hysteresis. */
    if (data->fuelPct < 10u)
    {
        fuelActive = 1u;
    }
    else if (data->fuelPct > 13u)
    {
        fuelActive = 0u;
    }

    s_fuelActive = fuelActive;

    if (fuelActive != 0u)
    {
        mask |= WarnBit(WARN_FUEL);
    }

    /* Over-speed warning uses hysteresis. */
    if (data->speedKmh > 120u)
    {
        overspeedActive = 1u;
    }
    else if (data->speedKmh < 115u)
    {
        overspeedActive = 0u;
    }

    s_overspeedActive = overspeedActive;

    if (overspeedActive != 0u)
    {
        mask |= WarnBit(WARN_OVERSPEED);
    }

    /* Body warnings use the current switch states and speed thresholds. */
    if (data->seatbelt != 0u && data->speedKmh > 10u)
    {
        mask |= WarnBit(WARN_SEATBELT);
    }

    if (data->doorOpen != 0u && data->speedKmh > 5u)
    {
        mask |= WarnBit(WARN_DOOR);
    }

    if (data->handbrake != 0u && data->speedKmh > 5u)
    {
        mask |= WarnBit(WARN_HANDBRAKE);
    }

    data->warnMask = mask;
    s_prevState = data->state;
}

Warn_t WRN_Highest(const CarData_t *data)
{
    Warn_t warning;
    uint16_t mask;

    if (data == NULL)
    {
        return WARN_NONE;
    }

    mask = WRN_Mask(data);

    for (warning = WARN_OIL; warning <= WARN_HANDBRAKE; warning++)
    {
        if ((mask & WarnBit(warning)) != 0u)
        {
            return warning;
        }
    }

    return WARN_NONE;
}

uint16_t WRN_Mask(const CarData_t *data)
{
    if (data == NULL)
    {
        return 0u;
    }

    return data->warnMask;
}
