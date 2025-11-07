#include "timer.h"

void Timer_Init(TIM_HandleTypeDef* htim, uint32_t prescaler, uint32_t period)
{
    htim->Init.Prescaler = prescaler;
    htim->Init.CounterMode = TIM_COUNTERMODE_UP;
    htim->Init.Period = period;
    htim->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    if (HAL_TIM_Base_Init(htim) != HAL_OK)
    {
        /* Initialization Error */
        Error_Handler();
    }
}

void LPTIM1_Callback(void)
{
    /* This function is called when LPTIM1 interrupt occurs */
    /* Add your custom handling code here */
}