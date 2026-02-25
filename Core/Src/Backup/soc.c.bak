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

//TODO: Implement SOC and SOH functions
uint32_t CalculateSOC(SystemMonitorValues_t *systemValues, uint32_t *time)
{
    //Placeholder implementation - return 100% SOC for now
    return 0;
}

uint16_t CalculateSOH(SystemMonitorValues_t *systemValues)
{
    // Placeholder implementation - return 100% SOH for now
    return 1000; // 100.0% in tenths of percent
}