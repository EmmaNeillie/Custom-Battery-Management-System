#ifndef CELL_H
#define CELL_H
#include <stdint.h>

// Cell Voltage (mV - millivolts)
#define MIN_VOLTAGE 2500
#define NOM_VOLTAGE 3600    
#define MAX_VOLTAGE 4200

// Capacity
#define NOM_CELL_CAPACITY 3000  //mAh
#define MODULE_CAPACITY_AH 27   //Ah
#define MODULE_CAPACITY_WH 97.2 //Wh

// Current (A)
#define FAST_CHARGE_CURRENT 45
#define DISCHARGE_CURRENT   270 //with 80 deg temp cutoff

// Other
#define MAX_CELL_ENERGY 0.4 //MJ
#define INITIAL_INT_IMPEDANCE 2.0 //mOhms
#define INT_FUSE 405 //A

// Temperature limits in Celsius (scaled by 10, e.g., 250 = 25.0°C)
typedef struct {
    struct {
        int32_t min;      // 0 = 0°C (scaled by 10)
        int32_t typ;      // 250 = 25.0°C
        int32_t max;      // 600 = 60.0°C
    } discharge;

    struct {
        int32_t min;      // 0 = 0°C
        int32_t typ;      // 250 = 25.0°C
        int32_t max;      // 450 = 45.0°C
    } charge;
} TemperatureLimits;

typedef struct {
    uint32_t voltage_mv;      // Voltage in millivolts (mV)
    int32_t temperature_c10;  // Temperature in tenths of °C (e.g., -400 = -40.0°C, 250 = 25.0°C)
} VoltTempPoint;

// Lookup table for voltage to temperature conversion
#define VOLTAGE_TO_TEMPERATURE_TABLE_SIZE 33
static const VoltTempPoint VOLTAGE_TO_TEMPERATURE_TABLE[VOLTAGE_TO_TEMPERATURE_TABLE_SIZE] = {
    {2440, -400}, {2420, -350}, {2400, -300}, {2380, -250},
    {2350, -200}, {2320, -150}, {2270, -100}, {2230, -50},
    {2170, 0},    {2110, 50},   {2050, 100},  {1990, 150},
    {1920, 200},  {1860, 250},  {1800, 300},  {1740, 350},
    {1680, 400},  {1630, 450},  {1590, 500},  {1550, 550},
    {1510, 600},  {1480, 650},  {1450, 700},  {1430, 750},
    {1400, 800},  {1380, 850},  {1370, 900},  {1350, 950},
    {1340, 1000}, {1330, 1050}, {1320, 1100}, {1310, 1150},
    {1300, 1200}
};

// Linear interpolation lookup: maps measured voltage (mV) to temperature (tenths of °C)
static inline int32_t voltageToTemperature(uint32_t v_mv)
{
    // Handle out of range cases
    if (v_mv >= VOLTAGE_TO_TEMPERATURE_TABLE[0].voltage_mv) {
        return VOLTAGE_TO_TEMPERATURE_TABLE[0].temperature_c10;
    }
    if (v_mv <= VOLTAGE_TO_TEMPERATURE_TABLE[VOLTAGE_TO_TEMPERATURE_TABLE_SIZE - 1].voltage_mv) {
        return VOLTAGE_TO_TEMPERATURE_TABLE[VOLTAGE_TO_TEMPERATURE_TABLE_SIZE - 1].temperature_c10;
    }

    // Linear interpolation
    for (uint32_t i = 1; i < VOLTAGE_TO_TEMPERATURE_TABLE_SIZE; ++i) {
        if (v_mv >= VOLTAGE_TO_TEMPERATURE_TABLE[i].voltage_mv) {
            const VoltTempPoint *hi = &VOLTAGE_TO_TEMPERATURE_TABLE[i - 1];
            const VoltTempPoint *lo = &VOLTAGE_TO_TEMPERATURE_TABLE[i];
            
            // temp = lo.temp + (hi.temp - lo.temp) * (v_mv - lo.v) / (hi.v - lo.v)
            int32_t temp_diff = hi->temperature_c10 - lo->temperature_c10;
            int32_t v_diff = (int32_t)(hi->voltage_mv - lo->voltage_mv);
            int32_t v_offset = (int32_t)(v_mv - lo->voltage_mv);
            return lo->temperature_c10 + (temp_diff * v_offset) / v_diff;
        }
    }

    return VOLTAGE_TO_TEMPERATURE_TABLE[VOLTAGE_TO_TEMPERATURE_TABLE_SIZE - 1].temperature_c10; 
}

static const float interp_volt[6] = {2.17, 1.92, 1.68, 1.59, 1.51, 1.48};
static const float interp_eq[14] = {-148.15, 321.48, -80, 173.6, -83.33, 180, -111.11, 226.67, -125, 248.75, -166.67, 311.67, -275, 472}; // m1,c1,m2,c2 etc

// Global temperature limits instance
extern TemperatureLimits tempLimits;

#endif // CELL_H