#ifndef SOC_H
#define SOC_H

#include <stddef.h>
#include <stdint.h>

#include "accumulator.h"
#include "cell.h"
#include "project.h"

#include <math.h>

typedef struct {
    uint32_t voltage_mv;  // Voltage in millivolts (mV)
    uint32_t soc_pct10;   // State of Charge in tenths of percent (0-1000 for 0.0%-100.0%)
} BatterySOCParameters;

uint32_t CalculateSOC(SystemMonitorValues_t *systemValues, uint32_t *time);
#endif // SOC_H
