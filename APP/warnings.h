#ifndef WARNINGS_H_
#define WARNINGS_H_

#include <stdint.h>
#include "cluster_fsm.h"

/* Runtime data, per README DD-01. */
typedef struct
{
    uint16_t speedKmh;         /* 0..250                                 */
    uint16_t rpm;              /* 0..8000                                */
    uint8_t  fuelPct;          /* 0..100                                 */
    int16_t  coolantC;         /* -40..130                               */
    uint16_t battmV;           /* 0..16000                               */
    uint8_t  oilBarX10;        /* 0..100  (0.0..10.0 bar)                */
    uint32_t odoMetres;        /* lifetime, metres                       */
    uint32_t tripMetres;       /* since last reset                       */
    uint16_t maxSpeedKmh;      /* session record                         */
    uint16_t avgSpeedKmh;      /* trip average                           */
    uint16_t warnMask;         /* bit per Warn_t, 1 = active             */
    uint8_t  lampByte;         /* what was last shifted to the 595       */
    uint8_t  turnLeft   : 1;
    uint8_t  turnRight  : 1;
    uint8_t  highBeam   : 1;
    uint8_t  handbrake  : 1;
    uint8_t  seatbelt   : 1;   /* 1 = unbuckled                          */
    uint8_t  doorOpen   : 1;
    uint8_t  engineRun  : 1;
    uint8_t  limpHome   : 1;
    uint8_t  state;            /* ClusterState_t                         */
    uint8_t  page;             /* DisplayPage_t                          */
    uint32_t ignitionSec;      /* seconds since key on                   */
} CarData_t;

/* Warning IDs, per README DD-03. */
typedef enum
{
    WARN_NONE = 0,
    WARN_OIL,
    WARN_BATT,
    WARN_COOLANT,
    WARN_CHECK,
    WARN_FUEL,
    WARN_OVERSPEED,
    WARN_SEATBELT,
    WARN_DOOR,
    WARN_HANDBRAKE
} Warn_t;

/* Initialize warning state. */
void WRN_Init(void);

/* Update warning activity from the current sensor snapshot. */
void WRN_Update(CarData_t *data);

/* Return the highest-priority active warning. */
Warn_t WRN_Highest(const CarData_t *data);

/* Return the current warning mask. */
uint16_t WRN_Mask(const CarData_t *data);

#endif /* WARNINGS_H_ */
