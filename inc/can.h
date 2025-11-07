#ifndef CAN_H
#define CAN_H

#include <stdint.h>
#include "stm32g4xx_hal.h"

void CAN_Init(void);
void CAN_Transmit(uint32_t id, uint8_t *data, uint8_t len);
void CAN_RxCallback(CAN_RxHeaderTypeDef *rxHeader, uint8_t *data);
void CAN_ErrorCallback(void);
void CAN_BusOffRecovery(void);

#endif /* CAN_H */