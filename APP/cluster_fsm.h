#ifndef CLUSTER_FSM_H
#define CLUSTER_FSM_H

#include <stdint.h>

/* Ignition states, per README DD-03. Owned entirely by this module. */
typedef enum
{
    CS_OFF = 0,
    CS_ACC,
    CS_IGNITION,
    CS_BULBCHECK,
    CS_CRANKING,
    CS_RUNNING,
    CS_LIMP_HOME,
    CS_STALLED
} ClusterState_t;

/* Inputs sampled once per 10 ms tick and fed into ClusterFSM_Update().
 * Debouncing/edge acquisition for the buttons is done by the input
 * layer (Task_Inputs); this module only needs the current debounced
 * level of each signal. */
typedef struct
{
    uint8_t  keyButton;       /* 1 = ignition key button currently held down   */
    uint8_t  startButton;     /* 1 = start button currently held down         */
    uint16_t rpm;             /* current engine RPM, from the tacho module    */
    uint8_t  criticalWarning; /* 1 = a critical warning (oil/coolant) active  */
} ClusterFsmInputs_t;

/* Initialize the FSM to CS_OFF. Call once at startup. */
void ClusterFSM_Init(void);

/* Advance the FSM by one 10 ms tick. Call from Task_FSM. */
void ClusterFSM_Update(const ClusterFsmInputs_t *inputs);

/* Current ignition state. */
ClusterState_t ClusterFSM_GetState(void);

#endif /* CLUSTER_FSM_H */