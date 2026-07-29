#ifndef ADC_H_
#define ADC_H_

#include <stdint.h>

/* ADC Channel Selection Definitions (PA0..PA7) */
typedef enum {
    ADC_CHANNEL_0 = 0U, /* Fuel Sensor (PA0 / ADC0) */
    ADC_CHANNEL_1 = 1U, /* Temp Sensor (PA1 / ADC1) */
    ADC_CHANNEL_2 = 2U,
    ADC_CHANNEL_3 = 3U,
    ADC_CHANNEL_4 = 4U,
    ADC_CHANNEL_5 = 5U,
    ADC_CHANNEL_6 = 6U,
    ADC_CHANNEL_7 = 7U,
    ADC_CHANNEL_COUNT
} Adc_Channel_t;

/* Reference Voltage Selection */
typedef enum {
    ADC_REF_AREF = 0U,
    ADC_REF_AVCC = 1U,  /* AVCC with external capacitor at AREF pin */
    ADC_REF_INTERNAL    /* Internal 2.56V Voltage Reference */
} Adc_Reference_t;

/**
 * @brief Initialize the ADC peripheral (AVCC reference, Prescaler 64 for 8 MHz CPU).
 */
void ADC_Init(void);

/**
 * @brief Start single conversion on the requested channel and wait for completion (blocking with timeout).
 * @param channel Analog channel index (0..7).
 * @return 10-bit raw ADC conversion result (0..1023), or 0xFFFF on error/timeout.
 */
uint16_t ADC_ReadChannel(Adc_Channel_t channel);

/**
 * @brief Start single conversion asynchronously on selected channel.
 * @param channel Analog channel index (0..7).
 */
void ADC_StartConversion(Adc_Channel_t channel);

/**
 * @brief Check if an asynchronous ADC conversion is currently in progress.
 * @return 1 if busy, 0 if conversion complete/idle.
 */
uint8_t ADC_IsBusy(void);

/**
 * @brief Read result of completed conversion without blocking.
 * @return 10-bit raw ADC result (0..1023).
 */
uint16_t ADC_GetResult(void);

#endif /* ADC_H_ */