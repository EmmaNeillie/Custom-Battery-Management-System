#include "adc.h"
#include "utilities.h"

void ADC_Init(void){
    // Configure the ADC peripheral
}

uint16_t ADC_ReadChannel(uint32_t channel){
    // Read the specified ADC channel and return the raw value
    return 0;
}

float CurrentRead(void){
    // Read current sensor value and convert to Amps
    uint16_t raw = ADC_ReadChannel(ADC_CHANNEL);

    return EMA_Filter((float)raw);
}