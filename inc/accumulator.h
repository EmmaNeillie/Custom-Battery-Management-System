#ifndef ACCUMULATOR_H
#define ACCUMULATOR_H

#define MAX_TRACTIVE_V 252          //V
#define NOM_TRACTIVE_V 216          //V
#define MIN_TRACTIVE_V 150          //V
#define ACCU_SERIES    60           //modules
#define ACCU_PARALLEL  9            //cells
#define TOTAL_PACK_CAPACITY 27      //Ah
#define TOTAL_PACK_CAPACITY 24.49   //MJ
#define MAX_DISCHARGE_BURST 30      //A
#define MAX_CHARGING_CURRENT 1.67   //C
#define NOM_TS_CURRENT 135          //A
#define MAX_TS_CURRENT 226          //A
#define MAX_CHARGING_CURRENT 45     //A
#define MAX_COMB_MOTOR_POWER 57000  //W 

struct BatteryTopology {
    int cellsInParallel;       // Number of cells connected in parallel (P)
    int cellsInSeries;         // Number of cells connected in series (S)
    int modulesPerSegment;     // Number of modules in each segment
    int numberOfSegments;      // Total number of segments in the pack
    float totalPackVoltage;    // Total voltage of the battery pack (V)
    float nominalCellVoltage;  // Nominal voltage of a single cell (V)
    float totalCapacity;       // Total capacity of the pack (Ah)
    int totalCellCount;        // Total number of cells in the pack
};

#endif // ACCUMULATOR_H