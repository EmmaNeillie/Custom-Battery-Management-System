this is the full library #include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "mgp_types.h"
#include "mgp_utilities.h"
#include "mgp_systick.h"
#include "mgp_i2c.h"
#include "mgp_i2c_private.h"

static bool I2C_IsStopGenerated(volatile void *base) {
    return !(((I2C_TypeDef *)base)->ISR & I2C_ISR_STOPF);
}

static bool I2C_IsTxFIFOFull(volatile void *base){
    return !(((I2C_TypeDef *)base)->ISR & I2C_ISR_TXE);
}

static bool I2C_IsRxFIFOEmpty(volatile void *base){
    return !(((I2C_TypeDef *)base)->ISR & I2C_ISR_RXNE);
}

static bool I2C_IsBusy(volatile void *base){
    return (((I2C_TypeDef *)base)->ISR & I2C_ISR_BUSY);
}

static bool I2C_IsTransferComplete(volatile void *base){
    return (((I2C_TypeDef *)base)->ISR & I2C_ISR_TC);
}

static bool I2C_IsTransferReloadCompleted(volatile void *base){
    return (((I2C_TypeDef *)base)->ISR & I2C_ISR_TCR);
}

static void I2C_Reset(stI2C_Handler_t *hdlr){
    //Performed by clearing the PE bit of the I2C_CR1 register
    //PE must be kept low during at least three APB clock cycles to perform a reset
    for (u32 i=0; i<32; i++){
        CLEAR_BIT(hdlr->Instance->CR1, I2C_CR1_PE);
    }
    SET_BIT(hdlr->Instance->CR1, I2C_CR1_PE);
}

static void I2C_TransferConfig(stI2C_Handler_t *hdlr, u32 slaveAddr, size_t transferSize, eI2C_TransferMode_t mode, eI2C_TransferCfg_t cfg) {
    assert(I2C_IS_SLAVE_ADDRESS_VALID(hdlr->AddressMode, slaveAddr));
    assert(IsEnum_eI2C_TransferMode(mode));
    assert(IsEnum_eI2C_TransferCfg(cfg));
    assert(transferSize <= I2C_MAX_TRANSFER_SIZE);

    // Set slave address. SADD[7:1] in 7bit mode, SADD[9:0] in 10bit mode
    // Set tranfer size (bytes)
    // Set read or write, start or stop
    // Set autoend/reload
    MODIFY_REG(hdlr->Instance->CR2, 
               I2C_CR2_SADD|I2C_CR2_ADD10|I2C_CR2_NBYTES|I2C_CR2_RELOAD|I2C_CR2_AUTOEND|I2C_CR2_RD_WRN|I2C_CR2_START|I2C_CR2_STOP, 
               (slaveAddr << (hdlr->AddressMode == I2C_ADDRESS_MODE_7BIT) | hdlr->AddressMode | (transferSize << I2C_CR2_NBYTES_Pos) | cfg | mode));
}

static eI2C_Status_t I2C_WaitStop(stI2C_Handler_t *hdlr){
    // Wait for STOP flag
    if(!MGP_Utilities_WhileUntil(I2C_IsStopGenerated, hdlr->Instance, hdlr->Timeout_ms))
        return I2C_STATUS_TIMEOUT_ERROR;    
    // Clear STOP flag
    SET_BIT(hdlr->Instance->ICR, I2C_ICR_STOPCF);
    
    return (eI2C_Status_t)(hdlr->Instance->ISR & I2C_ERRORS); 
}

static eI2C_Status_t I2C_Write(stI2C_Handler_t *restrict hdlr, const u8 *restrict data, size_t size){
    // Push all data to TXDR
    for(u32 i=0; i<size; i++){
        // Wait until the tx register is empty
        if(!MGP_Utilities_WhileUntil(I2C_IsTxFIFOFull, hdlr->Instance, hdlr->Timeout_ms))
            return (eI2C_Status_t)(I2C_STATUS_TIMEOUT_ERROR | (hdlr->Instance->ISR & I2C_ERRORS));
        // Send byte
        const u32 idx = (hdlr->ByteOrder == I2C_BYTE_ORDER_MSB_FIRST) ? (size-1-i) : i;
        hdlr->Instance->TXDR = data[idx];
    }
    
    return (eI2C_Status_t)(hdlr->Instance->ISR & I2C_ERRORS);
}

static eI2C_Status_t I2C_Read(stI2C_Handler_t *restrict hdlr, u8 *restrict data, size_t size){

    I2C_TypeDef* i2c = hdlr->Instance;

    if (size > I2C_MAX_TRANSFER_SIZE) {
      
        for (u32 offset = 0; offset < size; ) {
           u32 chunk_size = (size - offset > I2C_MAX_TRANSFER_SIZE) ? I2C_MAX_TRANSFER_SIZE : (size - offset);

            // Wait until bus is ready
            if (!MGP_Utilities_WhileUntil(!I2C_IsBusy, hdlr->Instance, hdlr->Timeout_ms))
                return (eI2C_Status_t)(I2C_STATUS_TIMEOUT_ERROR | (hdlr->Instance->ISR & I2C_ERRORS));

            // Configure CR2 for this chunk
            hdlr->Instance->CR2 =
                (devAddr << I2C_CR2_SADD_Pos) & I2C_CR2_SADD |           // Slave address
                (chunk_size << I2C_CR2_NBYTES_Pos) & I2C_CR2_NBYTES |    // Bytes to read
                I2C_CR2_RD_WRN |                                         // Read direction
                I2C_CR2_START |                                          // Start or repeated start
                ((offset + chunk_size >= size) ? I2C_CR2_AUTOEND : 0);   // AutoEND only on last chunk

            for (u32 i = 0; i < chunk_size; i++) {
                // Wait for RXNE (RX register not empty)
                if (!MGP_Utilities_WhileUntil(I2C_IsRxFIFOEmpty, hdlr->Instance, hdlr->Timeout_ms)) {
                    return (eI2C_Status_t)(I2C_STATUS_TIMEOUT_ERROR | (hdlr->Instance->ISR & I2C_ERRORS));
                }

                u32 global_idx = (hdlr->ByteOrder == I2C_BYTE_ORDER_MSB_FIRST) 
                    ? (size - 1 - (offset + i)) 
                    : (offset + i);

                data[global_idx] = hdlr->Instance->RXDR;
            }

            // Optional: wait for STOPF if AUTOEND used
            if (offset + chunk_size >= size) {
                if (!MGP_Utilities_WhileUntil(I2C_IsStopGenerated, hdlr->Instance, hdlr->Timeout_ms))
                    return (eI2C_Status_t)(I2C_STATUS_TIMEOUT_ERROR | (hdlr->Instance->ISR & I2C_ERRORS));

                hdlr->Instance->ICR |= I2C_ICR_STOPCF; // Clear STOP flag
            }

            offset += chunk_size;
        }

    } else {
          //SDA input fills the shift register
          //RXDR is filled with the shift register content when the shift register is full
          for(u32 i=0; i<size; i++){ 
              // Wait until there's something in the rx register
              if(!MGP_Utilities_WhileUntil(I2C_IsRxFIFOEmpty, hdlr->Instance, hdlr->Timeout_ms))
                  return (eI2C_Status_t)(I2C_STATUS_TIMEOUT_ERROR | (hdlr->Instance->ISR & I2C_ERRORS));
              // Fill buffer with received data
              const u32 idx = (hdlr->ByteOrder == I2C_BYTE_ORDER_MSB_FIRST) ? (size-1-i) : i;
              data[idx] = hdlr->Instance->RXDR;
              // In master mode NACK is automatically generated
          }
    }

    return (eI2C_Status_t)(hdlr->Instance->ISR & I2C_ERRORS);
}

 

void MGP_I2C_Init(stI2C_Handler_t *hdlr){
    (assert(hdlr));
    (assert(IS_I2C_ALL_INSTANCE(hdlr->Instance)));
    (assert(IsEnum_eI2C_ClockSource(hdlr->ClkSrc)));
    (assert(IsEnum_eI2C_AddressMode(hdlr->AddressMode)));
    (assert(hdlr->ClockPrescaler <= 0xF+1));
    (assert(hdlr->SCLHighPeriod <= 0xFF+1));
    (assert(hdlr->SCLLowPeriod <= 0xFF+1));
    // Currently only master mode is supported
    (assert(hdlr->Mode == I2C_MODE_MASTER));
    //(assert(hdlr->ByteOrder ==  I2C_BYTE_ORDER_MSB_FIRST));

    //Enable the clock
    if (hdlr->Instance == I2C1) {
        //Configure and enable the clock source
        MODIFY_REG(RCC->CCIPR, RCC_CCIPR_I2C1SEL, hdlr->ClkSrc);
        SET_BIT(RCC->APBENR1, RCC_APBENR1_I2C1EN);
        MODIFY_REG(SYSCFG->CFGR1, SYSCFG_CFGR1_I2C1_FMP, hdlr->IsFastModePlus ? SYSCFG_CFGR1_I2C1_FMP : 0);
    }
    #if defined(STM32C051xx) || defined(STM32C071xx) || defined(STM32C091xx) || defined(STM32C092xx)
    else if (hdlr->Instance == I2C2) {
        //Configure and enable the clock source
      
        SET_BIT(RCC->APBENR1, RCC_APBENR1_I2C2EN);
        MODIFY_REG(SYSCFG->CFGR1, SYSCFG_CFGR1_I2C2_FMP, hdlr->IsFastModePlus ? SYSCFG_CFGR1_I2C2_FMP : 0);
    }
    #endif 

    
    if(hdlr->IsFastModePlus){
    SYSCFG->CFGR1 |= SYSCFG_CFGR1_I2C_PB6_FMP | SYSCFG_CFGR1_I2C_PB7_FMP |
                     SYSCFG_CFGR1_I2C_PB8_FMP | SYSCFG_CFGR1_I2C_PB9_FMP;
    }
    CLEAR_REG(hdlr->Instance->CR1); 

    //Analog noise filter on the SDA & SCL inputs requires suppression of spikes shorter than 50ns
    //In Fast Mode Plus and fast mode
    //if ((hdlr->IsFastModePlus || hdlr->IsFastMode)) {
    //    SET_BIT(hdlr->Instance->CR1, I2C_CR1_ANFOFF);
    //} else {
     //   CLEAR_BIT(hdlr->Instance->CR1, I2C_CR1_ANFOFF);
    //} 

    //Digital noise filter on the SDA & SCL inputs
    //DNF[3:0] bitfield of the I2C_CR1 register
    //MODIFY_REG(hdlr->Instance->CR1, I2C_CR1_DNF, hdlr->ClockPrescaler << I2C_CR1_DNF_Pos);
    //TODO
    CLEAR_REG(hdlr->Instance->TIMINGR);
    SET_BIT(hdlr->Instance->TIMINGR, ((hdlr->ClockPrescaler-1) << I2C_TIMINGR_PRESC_Pos) & I2C_TIMINGR_PRESC);
    SET_BIT(hdlr->Instance->TIMINGR, ((hdlr->SCLHighPeriod-1) << I2C_TIMINGR_SCLH_Pos) & I2C_TIMINGR_SCLH);
    SET_BIT(hdlr->Instance->TIMINGR, ((hdlr->SCLLowPeriod-1) << I2C_TIMINGR_SCLL_Pos) & I2C_TIMINGR_SCLL);
    SET_BIT(hdlr->Instance->TIMINGR, ((hdlr->DataSetupTime-1) << I2C_TIMINGR_SCLDEL_Pos) & I2C_TIMINGR_SCLDEL);
    // Note, SDADEL can represent a data setup time of 0ticks (no -1 in the formula)
    SET_BIT(hdlr->Instance->TIMINGR, ((hdlr->DataHoldTime) << I2C_TIMINGR_SDADEL_Pos) & I2C_TIMINGR_SDADEL);

    SET_BIT(hdlr->Instance->CR1, I2C_CR1_PE); //Enable the peripheral

}

eI2C_Status_t MGP_I2C_Read(stI2C_Handler_t *restrict hdlr, u32 slaveAddr, const void *restrict reg, size_t regSize, void *restrict data, size_t dataSize){
    assert(hdlr);
    
    if(!MGP_Utilities_WhileUntil(I2C_IsBusy, hdlr->Instance, hdlr->Timeout_ms))
        return I2C_STATUS_BUSY;
    
    // Write the register address to the slave
    eI2C_Status_t status = MGP_I2C_Write(hdlr, slaveAddr, reg, regSize, NULL, 0);
    if(status)
        goto Terminate;
    

    for(s32 s=dataSize; s>0 && status==I2C_STATUS_OK; s-=I2C_MAX_TRANSFER_SIZE){
        // Generate a start write condition in the first batch
        const eI2C_TransferCfg_t cfg = (s == dataSize) ? I2C_TRANSFER_CFG_START_READ : I2C_TRANSFER_CFG_NO_STARTSTOP;
        // Don't complete transfer after rxing NBYTES of data if we're not finished
        // TODO: Errata 2.12.5 does not recommend using RELOAD in while reading
        const eI2C_TransferMode_t mode = (s <= I2C_MAX_TRANSFER_SIZE) ? I2C_TRANSFER_MODE_AUTOEND : I2C_TRANSFER_MODE_RELOAD;
        // Transfer max 255 bytes in an interation
        const size_t size = min(s, I2C_MAX_TRANSFER_SIZE);
        // Initiate interaction
        I2C_TransferConfig(hdlr, slaveAddr, s, mode, cfg);
        // Transfer batch
        status = I2C_Read(hdlr, &((u8*)data)[dataSize-s], s);
        
        // If automatic reload is used w8 for transfer complete
        if(mode == I2C_TRANSFER_MODE_RELOAD &&
           !MGP_Utilities_WhileUntil(I2C_IsTransferReloadCompleted, hdlr->Instance, hdlr->Timeout_ms)){
            status |= I2C_STATUS_TIMEOUT_ERROR | (hdlr->Instance->ISR & I2C_ERRORS);
       }
    }
    if(status)
        goto Terminate;
    
    // Wait for automatic STOP
    status = I2C_WaitStop(hdlr);
    if(status)
        goto Terminate;
    
    return status;
    
Terminate:
    // Reset the internal state machine, release the lines, clear errors
    I2C_Reset(hdlr);
    return status;
}

eI2C_Status_t MGP_I2C_Write(stI2C_Handler_t *restrict hdlr, u32 slaveAddr, const void *restrict reg, size_t regSize, const void *restrict data, size_t dataSize){
    assert(hdlr);

     
    if(!MGP_Utilities_WhileUntil(I2C_IsBusy, hdlr->Instance, hdlr->Timeout_ms))
        return I2C_STATUS_BUSY;

    const size_t txSize = regSize + dataSize;
    u8 *txData = malloc(txSize);
    assert(txData);

    // Tx message starts with reg address and then data
    memcpy(txData,         reg,  regSize);
    memcpy(txData+regSize, data, dataSize);

    eI2C_Status_t status = I2C_STATUS_OK;
    for(s32 s=txSize; s>0 && status==I2C_STATUS_OK; s-=I2C_MAX_TRANSFER_SIZE){
        // Generate a start write condition in the first batch
        const eI2C_TransferCfg_t cfg = (s == txSize) ? I2C_TRANSFER_CFG_START_WRITE : I2C_TRANSFER_CFG_NO_STARTSTOP;
        // Don't complete transfer after sending NBYTES of data if we're not finished
        const eI2C_TransferMode_t mode = (s <= I2C_MAX_TRANSFER_SIZE) ? I2C_TRANSFER_MODE_AUTOEND : I2C_TRANSFER_MODE_RELOAD;
        // Transfer max 255 bytes in an interation
        const size_t size = min(s, I2C_MAX_TRANSFER_SIZE);
        // Flush Tx FIFO in case some external EMI messed with I2C line
        SET_BIT(hdlr->Instance->ISR, I2C_ISR_TXE);
        // Initiate interaction
        I2C_TransferConfig(hdlr, slaveAddr, s, mode, cfg);
        // Transfer batch
        status = I2C_Write(hdlr, &txData[txSize-s], s);
        
        // If automatic reload is used w8 for transfer complete
        if(mode == I2C_TRANSFER_MODE_RELOAD &&
           !MGP_Utilities_WhileUntil(I2C_IsTransferReloadCompleted, hdlr->Instance, hdlr->Timeout_ms)){
            status |= I2C_STATUS_TIMEOUT_ERROR | (hdlr->Instance->ISR & I2C_ERRORS);
       }
    }
    free(txData);
    if(status)
        goto Terminate;
    
    // Wait for automatic STOP. No need to check TC flag.
    status = I2C_WaitStop(hdlr);
    if(status)
        goto Terminate;
    
    return status;
    
Terminate:
    // Reset the internal state machine, release the lines, clear errors
    I2C_Reset(hdlr);
    return status;
}