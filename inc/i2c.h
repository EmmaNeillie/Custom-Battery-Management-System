#ifndef I2C_H
#define I2C_H

#include "stm32g4xx_hal.h"

void I2C_Init(void);
HAL_StatusTypeDef I2C_Write(uint16_t DevAddress, uint8_t regAddr, uint8_t *data, uint16_t size);
HAL_StatusTypeDef I2C_Read(uint16_t DevAddress,uint8_t regAddr, uint8_t *data, uint16_t size);

typedef struct I2C_Handler{
    I2C_TypeDef     *Instance; 
    enum eI2C_Mode  Mode;
    enum eI2C_ClockSource ClkSrc;
    enum eI2C_AddressMode AddressMode;
    enum eI2C_ByteOrder ByteOrder;
    bool IsFastMode;
    u32 ClockPrescaler;
    u32 SCLHighPeriod;
    u32 SCLLowPeriod;
    u32 DataHoldTime;
    u32 DataSetupTime;
    u32 Timeout_ms;
} stI2C_Handler_t;

#endif // I2C_H