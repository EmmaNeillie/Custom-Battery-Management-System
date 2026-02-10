#ifndef SOC_H
#define SOC_H

#include <stddef.h>
#include <stdint.h>
#include "accumulator.h"
#include "cell.h"
#include "project.h"
#include <math.h>

#define INITIAL_FULL_CHARGE_V MAX_TRACTIVE_V
#define INITIAL_FULL_DISCHARGE_V MIN_TRACTIVE_V

uint32_t CalculateSOC(SystemMonitorValues_t *systemValues, uint32_t *time);
#endif // SOC_H
