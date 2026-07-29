#include "cluster_fsm.h"

/* Tick counts, all relative to the 10 ms rate of Task_FSM (README DD-05) */
#define BULBCHECK_TICKS         300u  /* 3 s bulb check                    */
#define CRANK_MAX_TICKS         500u  /* 5 s cranking limit                */
#define KEY_OFF_HOLD_TICKS      200u  /* 2 s key hold forces CS_OFF        */
#define RPM_RUN_CONFIRM_TICKS    50u  /* 500 ms of RPM above threshold     */
#define RPM_STALL_CONFIRM_TICKS 100u  /* 1 s of RPM below threshold        */

#define RPM_RUN_THRESHOLD   500u
#define RPM_STALL_THRESHOLD 300u

static ClusterState_t s_state;

/* Key button edge/hold tracking */
static uint8_t  s_keyPrevLevel;
static uint16_t s_keyHoldTicks;

/* Start button edge tracking */
static uint8_t s_startPrevLevel;

/* Per-state timers */
static uint16_t s_bulbCheckTicks;
static uint16_t s_crankTicks;
static uint16_t s_rpmRunTicks;
static uint16_t s_rpmStallTicks;

/* Returns 1 on the tick the key button transitions low->high */
static uint8_t KeyPressEdge(uint8_t keyLevel)
{
    uint8_t pressed = (keyLevel && !s_keyPrevLevel) ? 1u : 0u;
    s_keyPrevLevel = keyLevel;
    return pressed;
}

/* Returns 1 on the tick the start button transitions low->high */
static uint8_t StartPressEdge(uint8_t startLevel)
{
    uint8_t pressed = (startLevel && !s_startPrevLevel) ? 1u : 0u;
    s_startPrevLevel = startLevel;
    return pressed;
}

void ClusterFSM_Init(void)
{
    s_state = CS_OFF;

    s_keyPrevLevel   = 0;
    s_keyHoldTicks   = 0;
    s_startPrevLevel = 0;

    s_bulbCheckTicks = 0;
    s_crankTicks     = 0;
    s_rpmRunTicks    = 0;
    s_rpmStallTicks  = 0;
}

void ClusterFSM_Update(const ClusterFsmInputs_t *inputs)
{
    uint8_t keyEdge   = KeyPressEdge(inputs->keyButton);
    uint8_t startEdge = StartPressEdge(inputs->startButton);

    /* T12: from any state but CS_OFF, holding the key 2 s forces CS_OFF.
     * This is checked ahead of the per-state logic since it overrides it. */
    if (s_state != CS_OFF)
    {
        if (inputs->keyButton)
        {
            s_keyHoldTicks++;
        }
        else
        {
            s_keyHoldTicks = 0;
        }

        if (s_keyHoldTicks >= KEY_OFF_HOLD_TICKS)
        {
            /* TODO: odometer module must be signalled to force-save here */
            s_state          = CS_OFF;
            s_keyHoldTicks   = 0;
            s_bulbCheckTicks = 0;
            s_crankTicks     = 0;
            s_rpmRunTicks    = 0;
            s_rpmStallTicks  = 0;
            return;
        }
    }
    else
    {
        s_keyHoldTicks = 0;
    }

    switch (s_state)
    {
        case CS_OFF:
            if (keyEdge)
            {
                s_state = CS_ACC; /* T1 */
            }
            break;

        case CS_ACC:
            if (keyEdge)
            {
                /* T2 + T3: CS_IGNITION is a pass-through entry state that
                 * falls straight into the bulb check (README section 17.1) */
                s_state          = CS_BULBCHECK;
                s_bulbCheckTicks = 0;
            }
            break;

        case CS_BULBCHECK:
            s_bulbCheckTicks++;
            if (s_bulbCheckTicks >= BULBCHECK_TICKS)
            {
                s_state = CS_IGNITION; /* T4 */
            }
            break;

        case CS_IGNITION:
            if (startEdge)
            {
                s_state      = CS_CRANKING; /* T5 */
                s_crankTicks = 0;
            }
            break;

        case CS_CRANKING:
            s_crankTicks++;

            if (inputs->rpm > RPM_RUN_THRESHOLD)
            {
                s_rpmRunTicks++;
            }
            else
            {
                s_rpmRunTicks = 0;
            }

            if (s_rpmRunTicks >= RPM_RUN_CONFIRM_TICKS)
            {
                s_state       = CS_RUNNING; /* T6 */
                s_rpmRunTicks = 0;
                /* TODO: diagnostics module should log !EVT,ENGINE,START */
            }
            else if (s_crankTicks >= CRANK_MAX_TICKS)
            {
                s_state = CS_IGNITION; /* T7 */
                /* TODO: diagnostics module should log !EVT,CRANK,FAIL */
            }
            break;

        case CS_RUNNING:
            if (inputs->rpm < RPM_STALL_THRESHOLD)
            {
                s_rpmStallTicks++;
            }
            else
            {
                s_rpmStallTicks = 0;
            }

            if (s_rpmStallTicks >= RPM_STALL_CONFIRM_TICKS)
            {
                s_state         = CS_STALLED; /* T8 */
                s_rpmStallTicks = 0;
                /* TODO: diagnostics module should log !EVT,ENGINE,STALL */
            }
            else if (inputs->criticalWarning)
            {
                s_state = CS_LIMP_HOME; /* T10 */
                /* TODO: chime/diagnostics modules react to this state */
            }
            break;

        case CS_STALLED:
            if (startEdge)
            {
                s_state      = CS_CRANKING; /* T9 */
                s_crankTicks = 0;
            }
            break;

        case CS_LIMP_HOME:
            /* T11: only the key-hold check above can leave this state */
            break;

        default:
            /* Should never happen - fail safe instead of crashing */
            s_state = CS_OFF;
            break;
    }
}

ClusterState_t ClusterFSM_GetState(void)
{
    return s_state;
}