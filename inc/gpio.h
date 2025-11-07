#ifndef GPIO_H
#define GPIO_H

#include "stm32g4xx_hal.h"

void GPIO_Init(void);
void LED_Set(uint8_t led, uint8_t state);
uint8_t SDC_Status(void);
void ChargerEnable(uint8_t enable);

#endif /* GPIO_H */