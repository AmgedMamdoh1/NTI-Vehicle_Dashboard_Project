#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <stdint.h>

/* Minimal ignition state enum for the display module. */
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

/* Display page IDs, per README FR-17. */
typedef enum
{
    PG_MAIN = 0,
    PG_TRIP,
    PG_ENGINE,
    PG_ELECTRICAL,
    PG_DIAG
} DisplayPage_t;

/* Minimal runtime view used by the display module. */
typedef struct
{
    uint16_t speedKmh;
    uint16_t rpm;
    uint8_t  fuelPct;
    int16_t  coolantC;
    uint16_t battmV;
    uint8_t  oilBarX10;
    uint32_t odoMetres;
    uint32_t tripMetres;
    uint32_t tripSeconds;
    uint16_t avgSpeedKmh;
    uint16_t warnMask;
    ClusterState_t state;
    uint32_t ignitionSec;
} DisplayData_t;

/* Two-line LCD buffer. */
typedef struct
{
    char line1[17];
    char line2[17];
} DisplayLines_t;

/* Initialize the display manager. */
void Display_Init(void);

/* Render the current LCD content for the supplied state. */
void Display_Update(const DisplayData_t *data, DisplayLines_t *lines);

/* Select a specific page. */
void Display_SetPage(DisplayPage_t page);

/* Return the currently selected page. */
DisplayPage_t Display_GetPage(void);

/* Advance to the next page. */
void Display_NextPage(void);

#endif /* DISPLAY_H_ */
