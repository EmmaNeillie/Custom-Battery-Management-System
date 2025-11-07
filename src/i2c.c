#include "i2c.h"

void I2C_Init(void)
{
    /* Initialize I2C peripheral here */
}

HAL_StatusTypeDef I2C_Write(uint16_t DevAddress, uint8_t regAddr, uint8_t *data, uint16_t size)
{
    /* Implement I2C write operation here */
    return HAL_I2C_Mem_Write(&hi2c1, DevAddress, regAddr, I2C_MEMADD_SIZE_8BIT, data, size, HAL_MAX_DELAY);
}

HAL_StatusTypeDef I2C_Read(uint16_t DevAddress,uint8_t regAddr, uint8_t *data, uint16_t size)
{
    /* Implement I2C read operation here */
    return HAL_I2C_Mem_Read(&hi2c1, DevAddress, regAddr, I2C_MEMADD_SIZE_8BIT, data, size, HAL_MAX_DELAY);
}