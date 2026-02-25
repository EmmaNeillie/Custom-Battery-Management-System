#ifndef SYSTEM_MONITOR_H
#define SYSTEM_MONITOR_H

#include <stdbool.h>
#include "stm32g4xx_hal.h"
#include "accumulator.h"
#include "adc.h"
#include "cell.h"
#include "utilities.h"
#include "soc.h"
#include "charger.h"
#include "fdcan.h"
#include <string.h>

#define SYSTEM_MODULE_COUNT 14
#define TEMPERATURES_PER_MODULE 4
#define TOTAL_TEMPERATURES (SYSTEM_MODULE_COUNT * TEMPERATURES_PER_MODULE)

typedef enum {
    SYSTEM_STATE_NORMAL = 0,
    SYSTEM_STATE_FAULT
} SystemState_t;

typedef enum {
    CHARGE = 0,
    DISCHARGE
} SystemMode_t;

typedef struct SystemMonitorValues_t {
	float packVoltage; // volts 
	float packCurrent; // amps 
	float packPower;   // watts 
    float packSOC;
    float packSOH;
    float boardTemp; // degrees Celsius
    bool SDCstatus;
	SystemMode_t mode;
    uint32_t lastRxTime; 
} SystemMonitorValues_t;

typedef struct {
    float cellTemperatures[TOTAL_TEMPERATURES];
    uint32_t lastRxTime; 
} SystemTemperatureValues_t;

typedef struct {
    float cellVoltages[NOOFSEGMENTS][NOOFVOLTAGETAPS]; 
    uint32_t lastRxTime; 
} SystemVoltageValues_t;

// static SystemTemperatureValues_t temperatureValues;
// static SystemVoltageValues_t voltageValues;
// static SystemState_t currentState = SYSTEM_STATE_NORMAL;
extern SystemMonitorValues_t values;

void SystemMonitor_Init(void);
void SystemMonitor_Update(void);
void SystemMonitor_HandleRxCAN1(FDCAN_RxHeaderTypeDef *rxHeader, uint8_t *data);
void SystemMonitor_HandleRxCAN2(FDCAN_RxHeaderTypeDef *rxHeader, uint8_t *data);
void SystemMonitor_UpdateTemperatureData(CAN_MSG_SegTemps_t *tempData);
void SystemMonitor_UpdateCellVoltageData(CAN_MSG_CellVoltages_t *cellData, uint32_t id);
void SystemMonitor_CalculateTotalPackVoltage(float *packVoltage, SystemVoltageValues_t *voltageValues, BatteryTopology_t *topology);
void SystemMonitor_SendCAN(void);
void setSystemMode(SystemMode_t mode);

#endif /* SYSTEM_MONITOR_H */