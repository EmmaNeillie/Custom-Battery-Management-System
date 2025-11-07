#ifndef CELL_H
#define CELL_H

#include <string>
#include <array>
#include <cstddef>

// Cell Voltage (V)
#define MIN_VOLTAGE 2.5
#define NOM_VOLTAGE 3.6    
#define MAX_VOLTAGE 4.2

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

// Temperature limits in Celsius
struct TemperatureLimits {
    struct {
        const static double min = -20.0;
        const static double typ = 25.0;
        const static double max = 60.0;
    } discharge;

    struct {
        const static double min = 0.0;
        const static double typ = 25.0;
        const static double max = 45.0;
    } charge;
};

struct VoltTempPoint {
    double voltage;     // V
    double temperature; // °C
};

constexpr std::array<VoltTempPoint, 34> VOLTAGE_TO_TEMPERATURE_TABLE = {{
    {2.44, -40.0}, {2.42, -35.0}, {2.40, -30.0}, {2.38, -25.0},
    {2.35, -20.0}, {2.32, -15.0}, {2.27, -10.0}, {2.23, -5.0},
    {2.17, 0.0},   {2.11, 5.0},   {2.05, 10.0},  {1.99, 15.0},
    {1.92, 20.0},  {1.86, 25.0},  {1.80, 30.0},  {1.74, 35.0},
    {1.68, 40.0},  {1.63, 45.0},  {1.59, 50.0},  {1.55, 55.0},
    {1.51, 60.0},  {1.48, 65.0},  {1.45, 70.0},  {1.43, 75.0},
    {1.40, 80.0},  {1.38, 85.0},  {1.37, 90.0},  {1.35, 95.0},
    {1.34, 100.0}, {1.33, 105.0}, {1.32, 110.0}, {1.31, 115.0},
    {1.30, 120.0}
}};

// Linear interpolation lookup: maps measured voltage to temperature
constexpr double voltageToTemperature(double v)
{
    const auto& tbl = VOLTAGE_TO_TEMPERATURE_TABLE;

    if (v >= tbl.front().voltage) return tbl.front().temperature;
    if (v <= tbl.back().voltage)  return tbl.back().temperature;

    for (std::size_t i = 1; i < tbl.size(); ++i) {
        if (v >= tbl[i].voltage) {
            const auto& hi = tbl[i - 1];
            const auto& lo = tbl[i];
            double frac = (v - lo.voltage) / (hi.voltage - lo.voltage);
            return lo.temperature + frac * (hi.temperature - lo.temperature);
        }
    }

    return tbl.back().temperature; 
}

static_assert(voltageToTemperature(2.44) == -40.0, "Test failed: 2.44V");
static_assert(voltageToTemperature(1.31) == 115.0, "Test failed: 1.31V");
static_assert(voltageToTemperature(2.17) == 0.0, "Test failed: 2.17V");


#endif // CELL_H