
#ifndef BODYSW_H_
#define BODYSW_H_

#include <stdint.h>

/* Body Switch Identification Handles */
typedef enum {
    BODYSW_TURN_LEFT = 0, /* PB0: Active low                     */
    BODYSW_TURN_RIGHT,   /* PB1: Active low                     */
    BODYSW_HIGH_BEAM,    /* PB2: Active low                     */
    BODYSW_TRIP_RESET,   /* PB3: Active low                     */
    BODYSW_HANDBRAKE,    /* PC3: Active low                     */
    BODYSW_SEATBELT,     /* PC4: Active low (1 = unbuckled)     */
    BODYSW_DOOR,         /* PC5: Active low (1 = open)          */
    BODYSW_START_BTN,    /* PD4: Active low                     */
    BODYSW_DISP_CYCLE,   /* PD5: Active low                     */
    BODYSW_COUNT
} BodySw_Id_t;

/**
 * @brief Initialize body switch GPIO inputs and internal pull-ups.
 */
void BodySw_Init(void);

/**
 * @brief Sample and debounce all body switches. Called periodically (e.g. 10 ms).
 */
void BodySw_Update(void);

/**
 * @brief Get active logical state of a switch.
 * @param id Switch identifier.
 * @return 1 if active/pressed, 0 if inactive/released.
 */
uint8_t BodySw_GetState(BodySw_Id_t id);

/**
 * @brief Check if a switch was just activated (rising edge of active state).
 * Clears edge status upon reading.
 * @param id Switch identifier.
 * @return 1 if pressed since last check, 0 otherwise.
 */
uint8_t BodySw_HasPressed(BodySw_Id_t id);

/**
 * @brief Check if a switch was just deactivated (falling edge of active state).
 * Clears edge status upon reading.
 * @param id Switch identifier.
 * @return 1 if released since last check, 0 otherwise.
 */
uint8_t BodySw_HasReleased(BodySw_Id_t id);

#endif /* BODYSW_H_ */