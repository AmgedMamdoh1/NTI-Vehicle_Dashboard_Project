#ifndef ODOMETER_H_
#define ODOMETER_H_

#include <stdint.h>

/* Minimal EEPROM access interface for the odometer module. */
typedef struct
{
    uint8_t (*readBytes)(uint16_t address, uint8_t *buffer, uint16_t length);
    uint8_t (*writeBytes)(uint16_t address, const uint8_t *buffer, uint16_t length);
} OdometerStorage_t;

/* Initialize odometer state and restore the persisted values from EEPROM. */
void Odometer_Init(const OdometerStorage_t *storage);

/* Add distance in millimetres to the odometer and trip meter. */
void Odometer_AddDistance(uint32_t mm);

/* Advance the one-second save and trip timers. */
void Odometer_TickSecond(void);

/* Save the current odometer state immediately. */
void Odometer_SaveNow(void);

/* Force a save before key-off. */
void Odometer_OnKeyOff(void);

/* Reset the trip meter and trip time. */
void Odometer_ResetTrip(void);

/* Current odometer value in metres. */
uint32_t Odometer_GetTotal(void);

/* Current trip value in metres. */
uint32_t Odometer_GetTrip(void);

/* Current trip elapsed time in seconds. */
uint32_t Odometer_GetTripSeconds(void);

/* Average trip speed in km/h, integer maths. */
uint16_t Odometer_GetTripAvgKmh(void);

#endif /* ODOMETER_H_ */
