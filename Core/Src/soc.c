#include "soc.h"
#include "accumulator.h"

// State of Charge Estimation

// Method: Coloumb Counting - Integration of current over time to provide a relative measure of the 
// battery's charge level

/**  
 * A cell is fully charged when its terminal voltage during charge equals the manufacturer-specified 
 * voltage value MAX_VOLTAGE (cell.h) and the charge equals the manufacturer specified end of charge
 * current ... (cell.h). The SOC of a fully charged cell is defined to be 100%. On the other hand, a
 * cell is fully discharged when its Open-Circuit Voltage (OCV) equals the manufacture specified cut-
 * off voltage MIN_VOLTAGE (cell.h). The SOC of a full discharged cell is 0%.
 * 
 * SOC is a relative measurement of the energy available in the battery pack.
 * 
 * SOC[%] = Qreleaseable/Qnom * 100 
 * 
 * Depth of Discharge is the amount of charge that has been discharged from the cell relative to the 
 * nominal capacity.
 * 
 * DOD[%] = Qreleased/Qnom * 100
 * 
 * Total capacity is the charge removed as the cell is brought from full charge to fully discharged.
 * 
 * Qtotal = Qreleaseable + Qreleased
 * (not constant - will be effected by battery performance as the cells age) 
 * 
 * Coulomb counting tracks of Qin and Qout by integrating the TS current over time.
 * To estimate SOC:
 *  1. Calculate initial DOD0 as DOD0 = SOH - SOC0 (take the state of health of the battery and subtract the intial SOC)
 *  2. Calculate the change in DOD measuring and integrating the current through the battery over time
 *  3. Estimate the accumulated DOD using DOD(t) = DOD(t-tmeas) + delta.DOD
 *  4. Estimate the SOC subtracting DOD from the current SOH
 * 
 * {https://www.renesas.com/en/document/apn/coulomb-counting-and-state-charge-estimation-featuring-raa489206isl94216a-bfe}
*/

#define INITIAL_FULL_CHARGE_V MAX_TRACTIVE_V
#define INITIAL_FULL_DISCHARGE_V MIN_TRACTIVE_V

void SOCinit()

uint32_t CalculateSOC(SystemMonitorValues_t *systemValues, uint32_t *time)
{
    // 
    return 0;


    // size_t tableSize = sizeof(batterySocTable) / sizeof(batterySocTable[0]);

    // if (voltage_mv >= batterySocTable[0].voltage_mv) {
    //     return batterySocTable[0].soc_pct10;
    // }
    // if (voltage_mv <= batterySocTable[tableSize - 1].voltage_mv) {
    //     return batterySocTable[tableSize - 1].soc_pct10;
    // }

    // for (size_t i = 0; i < tableSize - 1; i++) {
    //     if (voltage_mv >= batterySocTable[i].voltage_mv && voltage_mv <= batterySocTable[i + 1].voltage_mv) {
    //         uint32_t v1 = batterySocTable[i].voltage_mv;
    //         uint32_t soc1 = batterySocTable[i].soc_pct10;
    //         uint32_t v2 = batterySocTable[i + 1].voltage_mv;
    //         uint32_t soc2 = batterySocTable[i + 1].soc_pct10;

    //         // Linear interpolation
    //         // soc = soc1 + (soc2 - soc1) * (voltage_mv - v1) / (v2 - v1)
    //         uint32_t soc = soc1 + ((soc2 - soc1) * (voltage_mv - v1)) / (v2 - v1);
    //         return soc;
    //     }
    // }

    // return 0; // Error case (0%)
}