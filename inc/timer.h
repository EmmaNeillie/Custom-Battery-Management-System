#ifndef TIMER_H
#define TIMER_H

#include "stm32g4xx_hal.h"

void Timer_Init(TIM_HandleTypeDef* htim, uint32_t prescaler, uint32_t period);
void LPTIM1_Callback(void);

#endif // TIMER_H