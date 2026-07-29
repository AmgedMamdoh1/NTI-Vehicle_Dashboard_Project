#ifndef TACHO_H_
#define TACHO_H_

#include <stdint.h>

void TAC_Init(void);
void TAC_OnPulse(void);
void TAC_Update250ms(void);
uint16_t TAC_GetRpm(void);
uint8_t TAC_IsValid(void);

#endif /* TACHO_H_ */
