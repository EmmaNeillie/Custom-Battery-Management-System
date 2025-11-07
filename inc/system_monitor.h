#ifndef SYSTEM_MONITOR_H
#define SYSTEM_MONITOR_H

#include "stm32g4xx_hal.h"


#define SYSTEM_MODULE_COUNT 14
#define TEMPERATURES_PER_MODULE 4
#define TOTAL_TEMPERATURES (SYSTEM_MODULE_COUNT * TEMPERATURES_PER_MODULE)

typedef enum {
    SYSTEM_STATE_NORMAL = 0,
    SYSTEM_STATE_BALANCING,
    SYSTEM_STATE_FAULT
} SystemState_t;

typedef struct {
	float packVoltage; // volts 
	float packCurrent; // amps 
	float packPower;   // watts 
    float packSOC;
    float packSOH;
    bool SDCstatus;
	uint32_t lastRxTime; /* HAL_GetTick() timestamp of last CAN-derived update */
} SystemMonitorValues_t;

typedef struct {
    float cellTemperatures[TOTAL_TEMPERATURES]; /* degrees Celsius
        indexed as: moduleIndex * TEMPERATURES_PER_MODULE + sensorIndex */
    uint32_t lastRxTime; /* HAL_GetTick() timestamp of last temperature update */
} SystemTemperatureValues_t;


static SystemTemperatureValues_t temperatureValues;
static SystemState_t currentState = SYSTEM_STATE_NORMAL;
static SystemMonitorValues_t values;

void SystemMonitor_Init(void);
void SystemMonitor_Update(void);
// Handle an incoming CAN message (called from CAN Rx callback/ISR context) 
void SystemMonitor_HandleCAN(CAN_RxHeaderTypeDef *rxHeader, uint8_t *data);
void SystemMonitor_SetTemperatures(const SystemTemperatureValues_t *inTemps);
void SystemMonitor_GetTemperatures(SystemTemperatureValues_t *outTemps);

#endif /* SYSTEM_MONITOR_H */