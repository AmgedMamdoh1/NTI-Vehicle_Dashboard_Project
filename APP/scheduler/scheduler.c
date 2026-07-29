#include "scheduler.h"

#include <stddef.h>

#define SCHEDULER_MAX_TASKS 16u
#define SCHEDULER_TICK_MS   10u

typedef struct
{
    uint8_t periodTicks;
    uint8_t offsetTicks;
    uint8_t phase;
    uint8_t active;
    SchedulerTask_t task;
} SchedulerEntry_t;

static SchedulerEntry_t s_tasks[SCHEDULER_MAX_TASKS];
static uint8_t s_taskCount;
static uint16_t s_tickCount;
static uint8_t s_initialized;

static void Scheduler_ResetPhase(void)
{
    uint8_t index;

    for (index = 0u; index < s_taskCount; index++)
    {
        s_tasks[index].phase = s_tasks[index].offsetTicks;
    }
}

void Scheduler_Init(void)
{
    uint8_t index;

    s_taskCount = 0u;
    s_tickCount = 0u;
    s_initialized = 0u;

    for (index = 0u; index < SCHEDULER_MAX_TASKS; index++)
    {
        s_tasks[index].periodTicks = 0u;
        s_tasks[index].offsetTicks = 0u;
        s_tasks[index].phase = 0u;
        s_tasks[index].active = 0u;
        s_tasks[index].task = NULL;
    }

    s_initialized = 1u;
}

void Scheduler_Tick(void)
{
    if (s_initialized == 0u)
    {
        return;
    }

    s_tickCount++;
}

void Scheduler_Run(void)
{
    uint8_t index;

    if (s_initialized == 0u)
    {
        return;
    }

    if (s_tickCount == 0u)
    {
        Scheduler_ResetPhase();
    }

    for (index = 0u; index < s_taskCount; index++)
    {
        SchedulerEntry_t *entry = &s_tasks[index];

        if (entry->active == 0u || entry->task == NULL)
        {
            continue;
        }

        if (entry->phase == 0u)
        {
            entry->task();
            entry->phase = entry->periodTicks;
        }
        else
        {
            entry->phase--;
        }
    }
}

void Scheduler_RegisterTask(uint8_t periodTicks, uint8_t offsetTicks, SchedulerTask_t task)
{
    SchedulerEntry_t *entry;

    if (s_taskCount >= SCHEDULER_MAX_TASKS || task == NULL)
    {
        return;
    }

    entry = &s_tasks[s_taskCount];
    entry->periodTicks = periodTicks;
    entry->offsetTicks = offsetTicks;
    entry->phase = offsetTicks;
    entry->active = 1u;
    entry->task = task;
    s_taskCount++;
}
