/* main.c - STM32G4 base project structure
 * Usage:
 *  - Implement MX_<PERIPH>_Init() functions (or replace with generated HAL Cube ones).
 *  - Define USE_FREERTOS to compile RTOS startup path (requires CMSIS-RTOS2/CMSIS-RTOS wrapper).
 */

#include "stm32g4xx_hal.h"
#include "project.h"
#include "scheduler.h"


int main(void)
{
    HAL_Init();

    SystemClock_Config();

    GPIO_Init();
    CAN_Init();
    ADC_Init();
    I2C_Init();
    TIM_Init();

    SystemMonitor_Init();
    Diagnostics_Init();

    Scheduler_Init();

    Scheduler_AddTask(SystemMonitor_Update, 1000);     // 1000ms 
    Scheduler_AddTask(Diagnostics_PerformCheck, 1000); // 1000ms 

    while (1)
    {
        Scheduler_RunTasks();
    }
}


