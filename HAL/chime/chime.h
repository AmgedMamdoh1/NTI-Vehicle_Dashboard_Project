#ifndef CHIME_H_
#define CHIME_H_

#include <stdint.h>

/* Chime Pattern Types */
typedef enum {
    CHIME_PATTERN_OFF = 0,
    CHIME_PATTERN_SINGLE_SHORT,  /* 100 ms ON                        */
    CHIME_PATTERN_SINGLE_LONG,   /* 500 ms ON                        */
    CHIME_PATTERN_DOUBLE_BEEP,   /* 100 ms ON, 100 ms OFF, 100 ms ON */
    CHIME_PATTERN_CONTINUOUS     /* Constant ON until stopped        */
} Chime_Pattern_t;

/**
 * @brief Initialize buzzer control pin (PD7 / OC2).
 */
void Chime_Init(void);

/**
 * @brief Play requested chime pattern.
 * @param pattern Desired chime pattern.
 */
void Chime_Play(Chime_Pattern_t pattern);

/**
 * @brief Immediately stop active chime playback.
 */
void Chime_Stop(void);

/**
 * @brief Periodic update function (called every 10 ms from task scheduler).
 */
void Chime_Update(void);

/**
 * @brief Check if chime playback is active.
 * @return 1 if playing, 0 if idle.
 */
uint8_t Chime_IsBusy(void);

#endif /* CHIME_H_ */