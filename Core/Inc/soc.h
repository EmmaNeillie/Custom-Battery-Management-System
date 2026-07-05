#ifndef SOC_H
#define SOC_H

#include <stddef.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

// This struct will be saved at each coulomb update
typedef struct {
    uint32_t startTime;
    float startingCapacity; // Ah
    float currentCapacity; // Ah
    float startingCharge_per; // %
    float currentCharge_per; // %
    float currentCurrent; // A
    float averageCurrent; // A  
} socState_t;

typedef struct {
    float estimatedCapacity_Ah;
    float internalResistance;
    float soh_capacity;
    float soh_resistance;
    float soh_total;

    float lastVoltage;
    float lastCurrent;
} sohState_t;

typedef struct SystemMonitorValues SystemMonitorValues_t;

#include "accumulator.h"
#include "cell.h"

#define INITIAL_FULL_CHARGE_V MAX_TRACTIVE_V
#define INITIAL_FULL_DISCHARGE_V MIN_TRACTIVE_V

#define PACK_CAPACITY_mAh   (TOTAL_PACK_CAPACITY)         // macro from accumulator.h (mAh)
#define PACK_CAPACITY_Ah    ( (float)(PACK_CAPACITY_mAh) / 1000.0f ) // Ah
#define SECONDS_PER_HOUR    3600.0f

#ifndef PACK_CAPACITY_Ah
#define PACK_CAPACITY_Ah 27.0f
#endif

void SOC_Init(uint32_t currentTime, float initialSOC_per);
void SOC_InitWrapper(void);
socState_t CalculateSOC(SystemMonitorValues_t *systemValues, uint32_t *time);
uint16_t CalculateSOH(SystemMonitorValues_t *systemValues);
#endif // SOC_H
