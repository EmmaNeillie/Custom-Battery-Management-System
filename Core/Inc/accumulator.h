#ifndef ACCUMULATOR_H
#define ACCUMULATOR_H
#include "cell.h"

#define ACCU_SERIES     60              //modules
#define ACCU_PARALLEL   9               //cells
#define NOOFSEGMENTS    4               //segments
#define NOOFVOLTAGETAPS 13              //voltage taps per segment


#define MAX_TRACTIVE_V ACCU_SERIES*MAX_VOLTAGE          //V
#define NOM_TRACTIVE_V ACCU_SERIES*NOM_VOLTAGE          //V
#define MIN_TRACTIVE_V ACCU_SERIES*MIN_VOLTAGE          //V
#define VOLTAGE_RANGE  MAX_TRACTIVE_V-MIN_TRACTIVE_V    //V

#define TOTAL_PACK_CAPACITY 27000   //mAh
#define TOTAL_PACK_CAPACITY 24.49   //MJ
#define MAX_DISCHARGE_BURST 30      //A
#define MAX_CHARGING_CURRENT 1.67   //C
#define NOM_TS_CURRENT 135          //A
#define MAX_TS_CURRENT 226          //A
#define MAX_CHARGING_CURRENT 45     //A
#define MAX_COMB_MOTOR_POWER 57000  //W 

typedef struct {
    int cellsInParallel;       // Number of cells connected in parallel (P)
    int cellsInSeries;         // Number of cells connected in series (S)
    int modulesPerSegment;     // Number of modules in each segment
    int numberOfSegments;      // Total number of segments in the pack
    float totalPackVoltage;    // Total voltage of the battery pack (V)
    float nominalCellVoltage;  // Nominal voltage of a single cell (V)
    float totalCapacity;       // Total capacity of the pack (Ah)
    int totalCellCount;        // Total number of cells in the pack
} BatteryTopology_t;

static BatteryTopology_t batteryTopology;

#endif // ACCUMULATOR_H