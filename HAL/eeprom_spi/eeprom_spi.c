
#include "eeprom_spi.h"

/* 25LC256 Instruction Opcodes */
#define EEPROM_CMD_WREN   0x06U  /* Write Enable                */
#define EEPROM_CMD_WRDI   0x04U  /* Write Disable               */
#define EEPROM_CMD_RDSR   0x05U  /* Read Status Register        */
#define EEPROM_CMD_WRSR   0x01U  /* Write Status Register       */
#define EEPROM_CMD_READ   0x03U  /* Read Data from Memory       */
#define EEPROM_CMD_WRITE  0x02U  /* Write Data to Memory        */

/* Status Register Bit Masks */
#define EEPROM_SR_WIP     0x01U  /* Write-In-Progress Bit       */

/* SPI Slave enum requirement from README DD-03 */
typedef enum {
    SPI_SLAVE_EEPROM = 0,
    SPI_SLAVE_LAMPS  = 1
} SpiSlave_t;

/* External MCAL / Shared SPI Bus declarations */
/* TODO: Remove extern declarations once MCAL spi.h is integrated */
extern void SPI_Acquire(SpiSlave_t slave);
extern void SPI_Release(void);
extern uint8_t SPI_TransmitByte(uint8_t data);

/* Private helper functions */
static void Eeprom_WriteEnable(void);

static void Eeprom_WriteEnable(void)
{
    SPI_Acquire(SPI_SLAVE_EEPROM);
    (void)SPI_TransmitByte(EEPROM_CMD_WREN);
    SPI_Release();
}

void Eeprom_Init(void)
{
    /* Hardware CS (PB4) and SPI initialization managed by MCAL */
}

uint8_t Eeprom_IsBusy(void)
{
    uint8_t status = 0U;

    SPI_Acquire(SPI_SLAVE_EEPROM);
    (void)SPI_TransmitByte(EEPROM_CMD_RDSR);
    status = SPI_TransmitByte(0xFFU);
    SPI_Release();

    return (status & EEPROM_SR_WIP) ? 1U : 0U;
}

void Eeprom_WaitReady(void)
{
    while (Eeprom_IsBusy() != 0U)
    {
        /* Poll status register until WIP clears */
    }
}

uint8_t Eeprom_Read(uint16_t addr, uint8_t *buf, uint16_t len)
{
    uint16_t i;

    if (buf == ((void *)0) || ((uint32_t)addr + len) > EEPROM_TOTAL_SIZE)
    {
        return 1U;
    }

    /* Ensure any previous write has completed */
    Eeprom_WaitReady();

    SPI_Acquire(SPI_SLAVE_EEPROM);

    /* Send READ command followed by 16-bit address */
    (void)SPI_TransmitByte(EEPROM_CMD_READ);
    (void)SPI_TransmitByte((uint8_t)(addr >> 8U));
    (void)SPI_TransmitByte((uint8_t)(addr & 0xFFU));

    /* Read sequence of data bytes */
    for (i = 0U; i < len; i++)
    {
        buf[i] = SPI_TransmitByte(0xFFU);
    }

    SPI_Release();

    return 0U;
}

uint8_t Eeprom_WritePage(uint16_t addr, const uint8_t *buf, uint16_t len)
{
    uint16_t i;
    uint16_t pageOffset;

    if (buf == ((void *)0) || len == 0U || ((uint32_t)addr + len) > EEPROM_TOTAL_SIZE)
    {
        return 1U;
    }

    /* Enforce 64-byte page boundary alignment */
    pageOffset = addr % EEPROM_PAGE_SIZE;
    if ((pageOffset + len) > EEPROM_PAGE_SIZE)
    {
        return 1U;
    }

    /* Wait for EEPROM ready before writing */
    Eeprom_WaitReady();

    /* Send WREN instruction */
    Eeprom_WriteEnable();

    SPI_Acquire(SPI_SLAVE_EEPROM);

    /* Send WRITE command and 16-bit address */
    (void)SPI_TransmitByte(EEPROM_CMD_WRITE);
    (void)SPI_TransmitByte((uint8_t)(addr >> 8U));
    (void)SPI_TransmitByte((uint8_t)(addr & 0xFFU));

    /* Shift data out */
    for (i = 0U; i < len; i++)
    {
        (void)SPI_TransmitByte(buf[i]);
    }

    SPI_Release();

    return 0U;
}

uint8_t Eeprom_Write(uint16_t addr, const uint8_t *buf, uint16_t len)
{
    uint16_t bytesToWrite;
    uint16_t pageOffset;

    if (buf == ((void *)0) || ((uint32_t)addr + len) > EEPROM_TOTAL_SIZE)
    {
        return 1U;
    }

    while (len > 0U)
    {
        pageOffset = addr % EEPROM_PAGE_SIZE;
        bytesToWrite = EEPROM_PAGE_SIZE - pageOffset;

        if (bytesToWrite > len)
        {
            bytesToWrite = len;
        }

        if (Eeprom_WritePage(addr, buf, bytesToWrite) != 0U)
        {
            return 1U;
        }

        addr += bytesToWrite;
        buf += bytesToWrite;
        len -= bytesToWrite;
    }

    return 0U;
}