#ifndef UTILS_H
#define UTILS_H

#include <cstdint>
#include "stm32g4xx_hal.h"

float EMA_Filter(float new_sample, float prev_filtered, float alpha);
float Median_Filter(float* samples, uint8_t sample_count);

#endif // UTILS_H