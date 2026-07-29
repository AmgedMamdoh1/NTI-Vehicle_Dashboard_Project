
#ifndef EEPROM_SPI_H_
#define EEPROM_SPI_H_

#include <stdint.h>

/* EEPROM Hardware Parameters */
#define EEPROM_PAGE_SIZE        64U
#define EEPROM_TOTAL_SIZE       32768U

/**
 * @brief Initialize the SPI EEPROM hardware interface.
 */
void Eeprom_Init(void);

/**
 * @brief Check if the EEPROM write cycle is in progress (RDSR WIP bit).
 * @return 1 if busy, 0 if ready.
 */
uint8_t Eeprom_IsBusy(void);

/**
 * @brief Poll RDSR until the write cycle completes.
 */
void Eeprom_WaitReady(void);

/**
 * @brief Read a sequence of bytes from EEPROM.
 * @param addr 16-bit start address (0x0000..0x7FFF).
 * @param buf Pointer to buffer to hold read data.
 * @param len Number of bytes to read.
 * @return 0 on success, 1 on error (null pointer or invalid range).
 */
uint8_t Eeprom_Read(uint16_t addr, uint8_t *buf, uint16_t len);

/**
 * @brief Write up to one page (64 bytes) without crossing page boundaries.
 * @param addr 16-bit start address.
 * @param buf Pointer to data to write.
 * @param len Number of bytes to write (must not cross 64-byte boundary).
 * @return 0 on success, 1 on error.
 */
uint8_t Eeprom_WritePage(uint16_t addr, const uint8_t *buf, uint16_t len);

/**
 * @brief Write an arbitrary length buffer to EEPROM handling page boundaries.
 * @param addr 16-bit start address.
 * @param buf Pointer to data to write.
 * @param len Number of bytes to write.
 * @return 0 on success, 1 on error.
 */
uint8_t Eeprom_Write(uint16_t addr, const uint8_t *buf, uint16_t len);

#endif /* EEPROM_SPI_H_ */