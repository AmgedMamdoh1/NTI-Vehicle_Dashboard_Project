#include "../../Service/STD_Types.h"
#include "../../Service/Bit_Math.h"
#include "spi_registers.h"
#include "spi_interface.h"

/* ================================================================================
 *  SPI DRIVER - IMPLEMENTATION
 *  ------------------------------------------------------------------------------
 *  The driver initializes the peripheral, exchanges bytes, and supports a
 *  transfer-complete callback for interrupt-based usage.
 * ============================================================================== */

#ifdef __AVR__
#include <avr/interrupt.h>
#else
#define ISR(vector) void vector(void)
#endif

#define SPI_DDRB_REG (*(volatile uint8_h *)0x37) /* DDRB register */

static SPI_CallBackType SPI_CallBack = NULL;

STD_ReturnType SPI_Init(const SPI_ConfigType *addConfig)
{
    if (addConfig == NULL)
    {
        return E_NOK;
    }

    if (addConfig->role == SPI_MASTER)
    {
        SET_BIT(SPI_DDRB_REG, SPI_MOSI_PIN);
        SET_BIT(SPI_DDRB_REG, SPI_SCK_PIN);
        SET_BIT(SPI_DDRB_REG, SPI_SS_PIN);
        CLR_BIT(SPI_DDRB_REG, SPI_MISO_PIN);
    }
    else
    {
        CLR_BIT(SPI_DDRB_REG, SPI_MOSI_PIN);
        CLR_BIT(SPI_DDRB_REG, SPI_SCK_PIN);
        CLR_BIT(SPI_DDRB_REG, SPI_SS_PIN);
        SET_BIT(SPI_DDRB_REG, SPI_MISO_PIN);
    }

    if (addConfig->role == SPI_MASTER)
    {
        SET_BIT(SPI_SPCR_REG, SPI_MSTR_BIT);
    }
    else
    {
        CLR_BIT(SPI_SPCR_REG, SPI_MSTR_BIT);
    }

    if (addConfig->polarity == SPI_CPOL_IDLE_HIGH)
    {
        SET_BIT(SPI_SPCR_REG, SPI_CPOL_BIT);
    }
    else
    {
        CLR_BIT(SPI_SPCR_REG, SPI_CPOL_BIT);
    }

    if (addConfig->phase == SPI_CPHA_SAMPLE_TRAILING)
    {
        SET_BIT(SPI_SPCR_REG, SPI_CPHA_BIT);
    }
    else
    {
        CLR_BIT(SPI_SPCR_REG, SPI_CPHA_BIT);
    }

    if (addConfig->dataOrder == SPI_LSB_FIRST)
    {
        SET_BIT(SPI_SPCR_REG, SPI_DORD_BIT);
    }
    else
    {
        CLR_BIT(SPI_SPCR_REG, SPI_DORD_BIT);
    }

    if (addConfig->role == SPI_MASTER)
    {
        if ((addConfig->clockRate & 0x01u) != 0u)
        {
            SET_BIT(SPI_SPCR_REG, SPI_SPR0_BIT);
        }
        else
        {
            CLR_BIT(SPI_SPCR_REG, SPI_SPR0_BIT);
        }

        if ((addConfig->clockRate & 0x02u) != 0u)
        {
            SET_BIT(SPI_SPCR_REG, SPI_SPR1_BIT);
        }
        else
        {
            CLR_BIT(SPI_SPCR_REG, SPI_SPR1_BIT);
        }

        if ((addConfig->clockRate & 0x04u) != 0u)
        {
            SET_BIT(SPI_SPSR_REG, SPI_SPI2X_BIT);
        }
        else
        {
            CLR_BIT(SPI_SPSR_REG, SPI_SPI2X_BIT);
        }
    }

    SET_BIT(SPI_SPCR_REG, SPI_SPE_BIT);
    return E_OK;
}

STD_ReturnType SPI_DeInit(void)
{
    CLR_BIT(SPI_SPCR_REG, SPI_SPE_BIT);
    CLR_BIT(SPI_SPCR_REG, SPI_SPIE_BIT);
    SPI_CallBack = NULL;
    return E_OK;
}

STD_ReturnType SPI_Transceive(uint8_h txByte, uint8_h *puint8Rx)
{
    SPI_SPDR_REG = txByte;

    while (GET_BIT(SPI_SPSR_REG, SPI_SPIF_BIT) == 0)
    {
    }

    (void)SPI_SPSR_REG;
    if (puint8Rx != NULL)
    {
        *puint8Rx = SPI_SPDR_REG;
    }
    else
    {
        (void)SPI_SPDR_REG;
    }

    return E_OK;
}

STD_ReturnType SPI_SendByte(uint8_h txByte)
{
    return SPI_Transceive(txByte, NULL);
}

STD_ReturnType SPI_ReceiveByte(uint8_h *rxByte)
{
    return SPI_Transceive(0xFFu, rxByte);
}

STD_ReturnType SPI_SendString(const uint8_h *pString)
{
    if (pString == NULL)
    {
        return E_NOK;
    }

    while (*pString != '\0')
    {
        if (SPI_SendByte(*pString) != E_OK)
        {
            return E_NOK;
        }
        pString++;
    }

    return E_OK;
}

STD_ReturnType SPI_SetCallBack(SPI_CallBackType callBack)
{
    if (callBack == NULL)
    {
        return E_NOK;
    }

    SPI_CallBack = callBack;
    SET_BIT(SPI_SPCR_REG, SPI_SPIE_BIT);
#ifdef __AVR__
    SET_BIT(SPI_SREG_REG, SPI_GLOBAL_INT_BIT);
#endif
    return E_OK;
}

ISR(SPI_STC_vect)
{
    uint8_h receivedByte = SPI_SPDR_REG;
    if (SPI_CallBack != NULL)
    {
        SPI_CallBack(receivedByte);
    }
}
