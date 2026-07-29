
#include "bodysw.h"

/* Port / Pin mapping per README Pin Map (Section 7) */
#define PORT_B_ID 1U
#define PORT_C_ID 2U
#define PORT_D_ID 3U

typedef struct {
    uint8_t port;
    uint8_t pin;
} PinMap_t;

/* Pin mappings according to section 7 of README */
static const PinMap_t s_switchPins[BODYSW_COUNT] = {
    [BODYSW_TURN_LEFT]  = { PORT_B_ID, 0U }, /* PB0 */
    [BODYSW_TURN_RIGHT] = { PORT_B_ID, 1U }, /* PB1 */
    [BODYSW_HIGH_BEAM]  = { PORT_B_ID, 2U }, /* PB2 */
    [BODYSW_TRIP_RESET] = { PORT_B_ID, 3U }, /* PB3 */
    [BODYSW_HANDBRAKE]  = { PORT_C_ID, 3U }, /* PC3 */
    [BODYSW_SEATBELT]   = { PORT_C_ID, 4U }, /* PC4 */
    [BODYSW_DOOR]       = { PORT_C_ID, 5U }, /* PC5 */
    [BODYSW_START_BTN]  = { PORT_D_ID, 4U }, /* PD4 */
    [BODYSW_DISP_CYCLE] = { PORT_D_ID, 5U }  /* PD5 */
};

/* State tracking variables for debouncing */
static uint8_t s_state;       /* Debounced current active state bitmap */
static uint8_t s_pressedEdge;  /* Rising edge bitmap */
static uint8_t s_releasedEdge; /* Falling edge bitmap */
static uint8_t s_sampleHistory[BODYSW_COUNT];

/* External MCAL DIO declarations */
/* TODO: Remove extern declarations once MCAL dio.h is integrated */
extern void DIO_SetPinDirection(uint8_t port, uint8_t pin, uint8_t direction);
extern void DIO_EnablePullUp(uint8_t port, uint8_t pin);
extern uint8_t DIO_GetPinLevel(uint8_t port, uint8_t pin);

void BodySw_Init(void)
{
    uint8_t i;

    s_state = 0U;
    s_pressedEdge = 0U;
    s_releasedEdge = 0U;

    for (i = 0U; i < (uint8_t)BODYSW_COUNT; i++)
    {
        s_sampleHistory[i] = 0xFFU; /* Default to released (active-low pull-up HIGH) */

        /* TODO: Configure pins as inputs with internal pull-ups using MCAL APIs */
        /* DIO_SetPinDirection(s_switchPins[i].port, s_switchPins[i].pin, INPUT); */
        /* DIO_EnablePullUp(s_switchPins[i].port, s_switchPins[i].pin);           */
    }
}

void BodySw_Update(void)
{
    uint8_t i;
    uint8_t rawActive;
    uint8_t pinLevel;
    uint8_t oldState = s_state;

    for (i = 0U; i < (uint8_t)BODYSW_COUNT; i++)
    {
        /* TODO: Read raw pin level from MCAL DIO */
        pinLevel = 1U; /* DIO_GetPinLevel(s_switchPins[i].port, s_switchPins[i].pin); */

        /* Shift sample history (2-sample active-low debounce filter) */
        s_sampleHistory[i] = (uint8_t)((s_sampleHistory[i] << 1U) | (pinLevel & 0x01U));

        /* Active-low inputs: 0x00 means consistently pulled LOW (active/pressed) */
        if ((s_sampleHistory[i] & 0x03U) == 0x00U)
        {
            s_state |= (uint8_t)(1U << i);
        }
        /* 0x03 means consistently pulled HIGH (inactive/released) */
        else if ((s_sampleHistory[i] & 0x03U) == 0x03U)
        {
            s_state &= (uint8_t)(~(1U << i));
        }
        else
        {
            /* Intermediate bouncing state, maintain current debounced state */
        }
    }

    /* Edge detection against debounced state transitions */
    s_pressedEdge |= (uint8_t)(s_state & (uint8_t)(~oldState));
    s_releasedEdge |= (uint8_t)((uint8_t)(~s_state) & oldState);
}

uint8_t BodySw_GetState(BodySw_Id_t id)
{
    if (id >= BODYSW_COUNT)
    {
        return 0U;
    }

    return ((s_state & (uint8_t)(1U << (uint8_t)id)) != 0U) ? 1U : 0U;
}

uint8_t BodySw_HasPressed(BodySw_Id_t id)
{
    uint8_t bitMask;

    if (id >= BODYSW_COUNT)
    {
        return 0U;
    }

    bitMask = (uint8_t)(1U << (uint8_t)id);

    if ((s_pressedEdge & bitMask) != 0U)
    {
        s_pressedEdge &= (uint8_t)(~bitMask); /* Clear edge after reading */
        return 1U;
    }

    return 0U;
}

uint8_t BodySw_HasReleased(BodySw_Id_t id)
{
    uint8_t bitMask;

    if (id >= BODYSW_COUNT)
    {
        return 0U;
    }

    bitMask = (uint8_t)(1U << (uint8_t)id);

    if ((s_releasedEdge & bitMask) != 0U)
    {
        s_releasedEdge &= (uint8_t)(~bitMask); /* Clear edge after reading */
        return 1U;
    }

    return 0U;
}