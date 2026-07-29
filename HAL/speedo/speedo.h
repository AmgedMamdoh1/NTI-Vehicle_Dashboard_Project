#ifndef SPEEDO_H_
#define SPEEDO_H_

#include <stdint.h>

/* Speedometer public API. */
void SPD_Init(void);
void SPD_Update(void);
void SPD_OnCapture(uint16_t captureTicks);
void SPD_OnOverflow(void);
uint16_t SPD_GetKmh(void);
uint8_t SPD_IsValid(void);

#endif /* SPEEDO_H_ */
