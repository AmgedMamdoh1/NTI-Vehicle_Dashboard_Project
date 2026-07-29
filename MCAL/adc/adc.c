#include "adc.h"
#include <avr/io.h>

/* Blocking read conversion timeout count to prevent infinite loop */
#define ADC_TIMEOUT_CYCLES  10000UL

void ADC_Init(void)
{
    /* Select AVCC with external capacitor at AREF pin (REFS1=0, REFS0=1) */
    ADMUX = (uint8_t)(1U << REFS0);

    /*
     * Enable ADC (ADEN) and set Prescaler to 64 (ADPS2=1, ADPS1=1, ADPS0=0)
     * For 8 MHz F_CPU, 8000 kHz / 64 = 125 kHz (within optimal 50 kHz - 200 kHz range)
     */
    ADCSRA = (uint8_t)((1U << ADEN) | (1U << ADPS2) | (1U << ADPS1));
}

void ADC_StartConversion(Adc_Channel_t channel)
{
    if (channel >= ADC_CHANNEL_COUNT)
    {
        return;
    }

    /* Preserve Reference selection bits (REFS1:REFS0) and write MUX4:0 channel */
    ADMUX = (uint8_t)((ADMUX & 0xE0U) | ((uint8_t)channel & 0x1FU));

    /* Start conversion (ADSC = 1) */
    ADCSRA |= (uint8_t)(1U << ADSC);
}

uint8_t ADC_IsBusy(void)
{
    /* ADSC remains 1 while conversion is running, clears automatically when complete */
    return ((ADCSRA & (1U << ADSC)) != 0U) ? 1U : 0U;
}

uint16_t ADC_GetResult(void)
{
    /* Read ADCL first, then ADCH (handled automatically by reading ADC 16-bit register) */
    return (uint16_t)ADC;
}

uint16_t ADC_ReadChannel(Adc_Channel_t channel)
{
    uint32_t timeout = ADC_TIMEOUT_CYCLES;

    if (channel >= ADC_CHANNEL_COUNT)
    {
        return 0xFFFFU;
    }

    ADC_StartConversion(channel);

    /* Wait for conversion completion with timeout protection */
    while (ADC_IsBusy() && (timeout > 0U))
    {
        timeout--;
    }

    if (timeout == 0U)
    {
        return 0xFFFFU; /* Timeout error flag */
    }

    return ADC_GetResult();
}