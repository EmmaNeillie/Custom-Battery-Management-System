#ifndef ADC_H
#define ADC_H

#include "stm32g4xx_hal.h"

/* Choose ADC instance and channel */
#ifndef ADC_INSTANCE
#define ADC_INSTANCE        ADC1
#endif

#ifndef ADC_CHANNEL
#define ADC_CHANNEL         ADC_CHANNEL_1  
#endif

void ADC_Init(void);
uint16_t ADC_ReadChannel(uint32_t channel);
float CurrentRead(void);

#endif /* ADC_H */