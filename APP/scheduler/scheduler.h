#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <stdint.h>

/* Task callback type. */
typedef void (*SchedulerTask_t)(void);

/* Initialize the scheduler state. */
void Scheduler_Init(void);

/* Advance the scheduler by one 10 ms tick. */
void Scheduler_Tick(void);

/* Run the tasks that are due at the current tick. */
void Scheduler_Run(void);

/* Register a periodic task callback. */
void Scheduler_RegisterTask(uint8_t periodTicks, uint8_t offsetTicks, SchedulerTask_t task);

#endif /* SCHEDULER_H_ */
