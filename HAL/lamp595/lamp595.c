
#include "lamp595.h"

/* TODO: Include existing MCAL headers when available in project */
/* #include "../MCAL/dio.h" */
/* #include "../MCAL/spi.h" */

/* Pin Definitions per README Pin Map */
#define LAMPS595_LATCH_PORT   PORT_C  /* PC2: 74HC595 RCLK (latch) */
#define LAMPS595_LATCH_PIN    PIN_2

/* SPI Slave enum requirement from README DD-03 */
typedef enum {
    SPI_SLAVE_EEPROM = 0,
    SPI_SLAVE_LAMPS  = 1
} SpiSlave_t;

/* External MCAL / Shared SPI Bus declarations */
/* TODO: Remove extern declarations once MCAL spi.h/dio.h are integrated */
extern void SPI_Acquire(SpiSlave_t slave);
extern void SPI_Release(void);
extern uint8_t SPI_TransmitByte(uint8_t data);
extern void DIO_SetPinDirection(uint8_t port, uint8_t pin, uint8_t direction);
extern void DIO_SetPinLevel(uint8_t port, uint8_t pin, uint8_t level);
extern void _delay_us(double us);

/* Private module state holding current lamp output byte */
static uint8_t s_lampByte = 0x00U;

void Lamps595_Init(void)
{
    /* TODO: Set PC2 as Output via DIO driver */
    /* DIO_SetPinDirection(LAMPS595_LATCH_PORT, LAMPS595_LATCH_PIN, OUTPUT); */
    /* DIO_SetPinLevel(LAMPS595_LATCH_PORT, LAMPS595_LATCH_PIN, LOW); */

    /* Ensure initial output state is 0 (all lamps dark) */
    s_lampByte = 0x00U;
    Lamps595_Update();
}

void Lamps595_SetLamp(uint8_t lamp_mask)
{
    s_lampByte |= lamp_mask;
}

void Lamps595_ClearLamp(uint8_t lamp_mask)
{
    s_lampByte &= (uint8_t)(~lamp_mask);
}

void Lamps595_WriteOutputs(uint8_t byte)
{
    s_lampByte = byte;
}

uint8_t Lamps595_GetOutputs(void)
{
    return s_lampByte;
}

void Lamps595_Update(void)
{
    /* Acquire shared SPI bus for 74HC595 */
    SPI_Acquire(SPI_SLAVE_LAMPS);

    /* Shift byte out over hardware SPI */
    (void)SPI_TransmitByte(s_lampByte);

    /* Release SPI bus before latching pulse per section 9.2 */
    SPI_Release();

    /* Strobe RCLK (PC2) rising edge to latch outputs (>= 1 us pulse) */
    /* DIO_SetPinLevel(LAMPS595_LATCH_PORT, LAMPS595_LATCH_PIN, HIGH); */
    /* _delay_us(1.0); */
    /* DIO_SetPinLevel(LAMPS595_LATCH_PORT, LAMPS595_LATCH_PIN, LOW); */
}

void Lamps595_BulbCheck(void)
{
    s_lampByte = LAMP_ALL_MASK;
    Lamps595_Update();
}