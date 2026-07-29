#include "app.h"

#include <stddef.h>

/* Minimal local declarations for the existing APP APIs. */
void ClusterFSM_Init(void);
void WRN_Init(void);
void Odometer_Init(const void *storage);
void Display_Init(void);
void Scheduler_Init(void);
void Scheduler_RegisterTask(unsigned char periodTicks, unsigned char offsetTicks, void (*task)(void));
void Scheduler_Tick(void);
void Scheduler_Run(void);

/* Task callbacks registered with the scheduler. */
static void App_TaskInputs(void)
{
    /* TODO: connect to the input layer when available. */
}

static void App_TaskFsm(void)
{
    /* TODO: connect to the FSM inputs from the input layer when available. */
}

static void App_TaskLamps(void)
{
    /* TODO: connect to warning evaluation and lamp refresh when available. */
}

static void App_TaskSpeed(void)
{
    /* TODO: connect to speed capture and odometer integration when available. */
}

static void App_TaskTacho(void)
{
    /* TODO: connect to RPM window closure when available. */
}

static void App_TaskLcd(void)
{
    /* TODO: connect to the display renderer when available. */
}

static void App_TaskGauges(void)
{
    /* TODO: connect to analog gauge acquisition when available. */
}

static void App_Task1Hz(void)
{
    /* TODO: connect to trip timing and chime scheduling when available. */
}

static void App_TaskReport(void)
{
    /* TODO: connect to diagnostics framing when available. */
}

static void App_TaskEeprom(void)
{
    /* TODO: connect to EEPROM persistence state machine when available. */
}

void App_Init(void)
{
    ClusterFSM_Init();
    WRN_Init();
    Odometer_Init(NULL);
    Display_Init();
    Scheduler_Init();

    Scheduler_RegisterTask((unsigned char)1u, (unsigned char)0u, App_TaskInputs);
    Scheduler_RegisterTask((unsigned char)1u, (unsigned char)0u, App_TaskFsm);
    Scheduler_RegisterTask((unsigned char)5u, (unsigned char)1u, App_TaskLamps);
    Scheduler_RegisterTask((unsigned char)10u, (unsigned char)2u, App_TaskSpeed);
    Scheduler_RegisterTask((unsigned char)25u, (unsigned char)3u, App_TaskTacho);
    Scheduler_RegisterTask((unsigned char)25u, (unsigned char)5u, App_TaskLcd);
    Scheduler_RegisterTask((unsigned char)50u, (unsigned char)4u, App_TaskGauges);
    Scheduler_RegisterTask((unsigned char)100u, (unsigned char)6u, App_Task1Hz);
    Scheduler_RegisterTask((unsigned char)500u, (unsigned char)8u, App_TaskReport);
    Scheduler_RegisterTask((unsigned char)1u, (unsigned char)9u, App_TaskEeprom);
}

void App_Run(void)
{
    Scheduler_Tick();
    Scheduler_Run();
}
