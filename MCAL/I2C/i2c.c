#include "../../Service/STD_Types.h"
#include "../../Service/Bit_Math.h"
#include "i2c_registers.h"
#include "i2c_interface.h"

/* ================================================================================
 *  I2C (TWI) DRIVER - IMPLEMENTATION
 *  ------------------------------------------------------------------------------
 *  The driver implements basic master/slave TWI operations using the AVR TWI
 *  hardware registers and the status codes defined in the register header.
 * ============================================================================== */

STD_ReturnType I2C_InitMaster(const I2C_MasterConfigType *addConfig)
{
    if (addConfig == NULL)
    {
        return E_NOK;
    }

    CLR_BIT(I2C_TWSR_REG, I2C_TWPS0_BIT);
    CLR_BIT(I2C_TWSR_REG, I2C_TWPS1_BIT);

    if (addConfig->sclFrequency > 0U)
    {
        uint32_h twbr = ((I2C_F_CPU / addConfig->sclFrequency) - 16U) / 2U;
        I2C_TWBR_REG = (u8)twbr;
    }
    else
    {
        return E_NOK;
    }

    SET_BIT(I2C_TWCR_REG, I2C_TWEN_BIT);
    return E_OK;
}

STD_ReturnType I2C_InitSlave(const I2C_SlaveConfigType *addConfig)
{
    if (addConfig == NULL)
    {
        return E_NOK;
    }

    I2C_TWAR_REG = (u8)(addConfig->ownAddress << 1);
    if (addConfig->enableGeneralCall != 0U)
    {
        SET_BIT(I2C_TWAR_REG, I2C_TWGCE_BIT);
    }
    else
    {
        CLR_BIT(I2C_TWAR_REG, I2C_TWGCE_BIT);
    }

    I2C_TWCR_REG = (1u << I2C_TWINT_BIT) | (1u << I2C_TWEA_BIT) | (1u << I2C_TWEN_BIT);
    return E_OK;
}

STD_ReturnType I2C_DeInit(void)
{
    CLR_BIT(I2C_TWCR_REG, I2C_TWEN_BIT);
    return E_OK;
}

STD_ReturnType I2C_Start(void)
{
    I2C_TWCR_REG = (1u << I2C_TWINT_BIT) | (1u << I2C_TWSTA_BIT) | (1u << I2C_TWEN_BIT);

    while (GET_BIT(I2C_TWCR_REG, I2C_TWINT_BIT) == 0)
    {
    }

    if ((I2C_GetStatus() == I2C_STATUS_START) || (I2C_GetStatus() == I2C_STATUS_REP_START))
    {
        return E_OK;
    }

    return E_NOK;
}

STD_ReturnType I2C_Stop(void)
{
    I2C_TWCR_REG = (1u << I2C_TWINT_BIT) | (1u << I2C_TWSTO_BIT) | (1u << I2C_TWEN_BIT);

    while ((I2C_TWCR_REG & (1u << I2C_TWSTO_BIT)) != 0u)
    {
    }

    return E_OK;
}

STD_ReturnType I2C_WriteByte(uint8_h uint8Data)
{
    I2C_TWDR_REG = uint8Data;
    I2C_TWCR_REG = (1u << I2C_TWINT_BIT) | (1u << I2C_TWEN_BIT);

    while (GET_BIT(I2C_TWCR_REG, I2C_TWINT_BIT) == 0)
    {
    }

    return E_OK;
}

STD_ReturnType I2C_ReadByteWithAck(uint8_h *puint8Data)
{
    if (puint8Data == NULL)
    {
        return E_NOK;
    }

    I2C_TWCR_REG = (1u << I2C_TWINT_BIT) | (1u << I2C_TWEN_BIT) | (1u << I2C_TWEA_BIT);

    while (GET_BIT(I2C_TWCR_REG, I2C_TWINT_BIT) == 0)
    {
    }

    *puint8Data = I2C_TWDR_REG;
    return E_OK;
}

STD_ReturnType I2C_ReadByteWithNack(uint8_h *puint8Data)
{
    if (puint8Data == NULL)
    {
        return E_NOK;
    }

    I2C_TWCR_REG = (1u << I2C_TWINT_BIT) | (1u << I2C_TWEN_BIT);

    while (GET_BIT(I2C_TWCR_REG, I2C_TWINT_BIT) == 0)
    {
    }

    *puint8Data = I2C_TWDR_REG;
    return E_OK;
}

uint8_h I2C_GetStatus(void)
{
    return (uint8_h)(I2C_TWSR_REG & I2C_TWSR_STATUS_MASK);
}

STD_ReturnType I2C_MasterWrite(uint8_h slaveAddress, const uint8_h *pData, uint16_h length)
{
    uint16_h i;

    if (pData == NULL)
    {
        return E_NOK;
    }

    if (I2C_Start() != E_OK)
    {
        return E_NOK;
    }

    if (I2C_WriteByte((uint8_h)((slaveAddress << 1) | 0u)) != E_OK)
    {
        I2C_Stop();
        return E_NOK;
    }
    if (I2C_GetStatus() != I2C_STATUS_MT_SLA_ACK)
    {
        I2C_Stop();
        return E_NOK;
    }

    for (i = 0u; i < length; ++i)
    {
        if (I2C_WriteByte(pData[i]) != E_OK)
        {
            I2C_Stop();
            return E_NOK;
        }
        if (I2C_GetStatus() != I2C_STATUS_MT_DATA_ACK)
        {
            I2C_Stop();
            return E_NOK;
        }
    }

    I2C_Stop();
    return E_OK;
}

STD_ReturnType I2C_MasterRead(uint8_h slaveAddress, uint8_h *pBuffer, uint16_h length)
{
    uint16_h i;

    if ((pBuffer == NULL) || (length == 0u))
    {
        return E_NOK;
    }

    if (I2C_Start() != E_OK)
    {
        return E_NOK;
    }

    if (I2C_WriteByte((uint8_h)((slaveAddress << 1) | 1u)) != E_OK)
    {
        I2C_Stop();
        return E_NOK;
    }
    if (I2C_GetStatus() != I2C_STATUS_MR_SLA_ACK)
    {
        I2C_Stop();
        return E_NOK;
    }

    for (i = 0u; i < length; ++i)
    {
        if (i == (length - 1u))
        {
            if (I2C_ReadByteWithNack(&pBuffer[i]) != E_OK)
            {
                I2C_Stop();
                return E_NOK;
            }
        }
        else
        {
            if (I2C_ReadByteWithAck(&pBuffer[i]) != E_OK)
            {
                I2C_Stop();
                return E_NOK;
            }
        }
    }

    I2C_Stop();
    return E_OK;
}
