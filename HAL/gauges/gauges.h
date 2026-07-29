#ifndef GAUGES_H_
#define GAUGES_H_

#include <stdint.h>

void GAU_Init(void);
void GAU_Update(void);
uint8_t GAU_Fuel(void);
int16_t GAU_CoolantC(void);
uint16_t GAU_BattmV(void);
uint8_t GAU_OilBarX10(void);
uint8_t GAU_IsValid(uint8_t channel);

#endif /* GAUGES_H_ */
