#include "soc.h"
#include "system_monitor.h"

// State of Charge Estimation

// Internal state (static to this module)
static float soc_lastCurrent = 0.0f; // A - previous current for trapezoid integration
static uint32_t soc_initialized = 0;

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

// Initialize SOC state. Optionally pass initial SOC% (0..100). If unspecified use 100%.
void SOC_Init(uint32_t currentTime, float initialSOC_per)
{
    memset(&socState, 0, sizeof(socState));
    memset(&sohState, 0, sizeof(sohState));
    if (initialSOC_per < 0.0f) initialSOC_per = 0.0f;
    if (initialSOC_per > 100.0f) initialSOC_per = 100.0f;

    // Initialize any necessary variables or data structures for SOC estimation
    socState.startTime = currentTime;
    socState.startingCapacity = PACK_CAPACITY_Ah;
    socState.currentCapacity = PACK_CAPACITY_Ah * (initialSOC_per / 100.0f);
    socState.startingCharge_per = initialSOC_per;
    socState.currentCharge_per = initialSOC_per;
    socState.currentCurrent = 0.0f;
    socState.averageCurrent = 0.0f;
    soc_lastCurrent = 0.0f;
    soc_initialized = 1;

    sohState.estimatedCapacity_Ah = PACK_CAPACITY_Ah;
    sohState.internalResistance = PACK_INITIAL_RESISTANCE;
    sohState.soh_capacity = 100.0f;
    sohState.soh_resistance = 100.0f;
    sohState.soh_total = 100.0f;
}

// convenience wrapper: default to 100% if not given
void SOC_InitWrapper(void)
{
    SOC_Init(0, 100.0f);
}

/*
 * CalculateSOC
 *  - systemValues->packCurrent is expected in Amps (A)
 *  - time_ms is pointer to current time in milliseconds (monotonic)
 *  - sign convention: packCurrent > 0 means discharge (reduces SOC).
 *
 * Returns the currentState_t (by value).
 */
socState_t CalculateSOC(SystemMonitorValues_t *systemValues, uint32_t *time)
{
    if (!soc_initialized) {
        // If not initialized, choose starting point
        SOC_Init(*time, 100.0f);
    }

    uint32_t now = *time;
    uint32_t last = socState.startTime;

    uint32_t deltaTime_ms = now - last;
    if (deltaTime_ms == 0) {
        // Avoid division by zero, return current state
        return socState;
    }

    float currentNow = systemValues ? systemValues->packCurrent : 0.0f; // A

    if (systemValues && systemValues->mode == CHARGE){
        currentNow = -currentNow;
    }

    // Trapezoidal integration to calculate change in charge
    float deltaQ_Ah = 0.0f;
    if (deltaTime_ms > 0) {
        float dt_s = (float)deltaTime_ms / 1000.0f; // Convert ms to seconds
        deltaQ_Ah = ((soc_lastCurrent + currentNow) * 0.5f) * (dt_s / SECONDS_PER_HOUR); // Ah
    }

    //Update stored state
    socState.currentCapacity -= deltaQ_Ah; // Ah

    //Bound capacity
    if (socState.currentCapacity < 0.0f) socState.currentCapacity = 0.0f;
    if (socState.currentCapacity > socState.startingCapacity) socState.currentCapacity = socState.startingCapacity;

    //Update percentage
    if (socState.startingCapacity > 0.0f) {
        socState.currentCharge_per = (socState.currentCapacity / socState.startingCapacity) * 100.0f;
    }
    else {
        socState.currentCharge_per = 0.0f; // Avoid division by zero
    }

    // Update last current and time for next iteration
    socState.currentCurrent = currentNow;
    const float alpha = 0.1f; // Smoothing factor for average current
    socState.averageCurrent = (1.0f - alpha) * socState.averageCurrent + alpha * currentNow; // Exponential moving average

    soc_lastCurrent = currentNow;
    socState.startTime = now;

    // Percentage Clamp
    if (socState.currentCharge_per < 0.0f) socState.currentCharge_per = 0.0f;
    if (socState.currentCharge_per > 100.0f) socState.currentCharge_per = 100.0f;

    return socState;
}

uint16_t CalculateSOH(SystemMonitorValues_t *systemValues)
{
    if (!systemValues) {
        return (uint16_t)(sohState.soh_total * 10.0f); // Return latest estimated SOH
    }

    float voltage = systemValues->packVoltage;
    float current = systemValues->packCurrent;

    float dV = voltage - sohState.lastVoltage;
    float dI = current - sohState.lastCurrent;

    // Internal Resistance Estimation
    if (fabsf(dI) > 1.0f) // detect current step
    {
        float R_sample = fabsf(dV / dI);
        const float alpha = 0.02f;
        sohState.internalResistance =(1.0f - alpha) * sohState.internalResistance + alpha * R_sample;
    }

    // Capacity Estimation
    float usedCapacity = socState.startingCapacity - socState.currentCapacity;
    if (usedCapacity > 0.1f) // valid window
    {
        const float beta = 0.001f;
        sohState.estimatedCapacity_Ah =(1.0f - beta) * sohState.estimatedCapacity_Ah + beta * usedCapacity;
    }

    sohState.soh_capacity = (sohState.estimatedCapacity_Ah / PACK_CAPACITY_Ah) * 100.0f;
    sohState.soh_resistance = (PACK_INITIAL_RESISTANCE / sohState.internalResistance) * 100.0f;
    sohState.soh_total = 0.7f * sohState.soh_capacity + 0.3f * sohState.soh_resistance;

    if (sohState.soh_total > 100.0f) sohState.soh_total = 100.0f;
    if (sohState.soh_total < 0.0f) sohState.soh_total = 0.0f;

    sohState.lastVoltage = voltage;
    sohState.lastCurrent = current;

    return (uint16_t)(sohState.soh_total * 10.0f);
}

