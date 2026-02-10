#include "system_monitor.h"

void SystemMonitor_Init(void)
{
    // Initialise system monitor values
    values.packVoltage = 0.0f;
    values.packCurrent = 0.0f;
    values.packPower = 0.0f;
    values.packSOC = 100.0f;
    values.packSOH = 100.0f;
    values.boardTemp = 25.0f;
    values.SDCstatus = true;
    values.lastRxTime = 0;
    values.mode = DISCHARGE;

    // Initialise battery topology
    batteryTopology.cellsInParallel = ACCU_PARALLEL;
    batteryTopology.cellsInSeries = ACCU_SERIES;
    batteryTopology.modulesPerSegment = NOOFVOLTAGETAPS;
    batteryTopology.numberOfSegments = NOOFSEGMENTS;
    batteryTopology.totalPackVoltage = NOM_TRACTIVE_V;
    batteryTopology.nominalCellVoltage = (float)NOM_TRACTIVE_V / (float)(ACCU_SERIES);
    batteryTopology.totalCapacity = TOTAL_PACK_CAPACITY;
    batteryTopology.totalCellCount = ACCU_SERIES * ACCU_PARALLEL;
    
    currentState = SYSTEM_STATE_NORMAL;
    memset(&temperatureValues, 0, sizeof(temperatureValues));
    memset(&voltageValues, 0, sizeof(voltageValues));
}

void setSystemMode(SystemMode_t mode){
    values.mode = mode;
}


// System Monitor Update

// To be updated:
// - pack current from current sensor
// - pack voltage from cell voltages
// - pack power calculation
// - SOC
// - board temp from I2C sensor

void SystemMonitor_Update(void)
{
    // PACK CURRENT
    TSCurrentConverted_t TScurrent = ADC2_GetTSCurrent();
    SystemMonitor_CurrentFaultDetection(TScurrent); 
    values.packCurrent = (float)TScurrent.ch1_current / 1000.0f; // Convert mA to A

    // PACK VOLTAGE
    SystemMonitor_CalculateTotalPackVoltage(&values.packVoltage, &voltageValues, &batteryTopology);
    
    // PACK POWER
    values.packPower = values.packVoltage * values.packCurrent;
    
    // TODO: Implement SOC & SOH calculation
    // STATE OF CHARGE (SOC) & STATE OF HEALTH (SOH)
    CalculateSOC(&values, HAL_GetTick());
    CalculateSOH(&values);

    // BOARD TEMPERATURE
    values.boardTemp = I2C3_SensorRead();

    SystemMonitor_SendCAN();
}

void SystemMonitor_HandleRxCAN1(FDCAN_RxHeaderTypeDef *rxHeader, uint8_t *data)
{
    if (rxHeader == NULL || data == NULL) return;

    switch (rxHeader->Identifier) {
        case CAN_MSG_ID_CELL_VOLTAGES_1:
        case CAN_MSG_ID_CELL_VOLTAGES_2:
        case CAN_MSG_ID_CELL_VOLTAGES_3: {   /* Cell Voltages */
            CAN_MSG_CellVoltages_t *cellData = (CAN_MSG_CellVoltages_t *)data;
            SystemMonitor_UpdateCellVoltageData(cellData, rxHeader->Identifier);
            break;
        }

        case CAN_MSG_ID_TEMPERATURES: { /* Cell Temperatures */
            CAN_MSG_SegTemps_t *tempData = (CAN_MSG_SegTemps_t *)data;
            SystemMonitor_UpdateTemperatureData(tempData);
            break;
        }

        default:
            break;
    }
    values.lastRxTime = HAL_GetTick();
    if (currentState == SYSTEM_STATE_FAULT) {
        values.SDCstatus = false;
    } else {
        values.SDCstatus = true;
    }
}

void SystemMonitor_HandleRxCAN2(FDCAN_RxHeaderTypeDef *rxHeader, uint8_t *data){

    if (rxHeader == NULL || data == NULL) return;

    switch (values.mode) {
        case CHARGE:
            switch (rxHeader->Identifier) {
                case CHARGER_MSG2_ID: {
                    break; 
                }
                default: {
                    break;
                }
            }
            break;
    }
    values.lastRxTime = HAL_GetTick();
    if (currentState == SYSTEM_STATE_FAULT) {
        values.SDCstatus = false;
    } else {
        values.SDCstatus = true;
    }
}

void SystemMonitor_UpdateCellVoltageData(CAN_MSG_CellVoltages_t *cellData, uint32_t id)
{
    if (cellData == NULL) return;
    int index = 0;
    switch (id) {
        case CAN_MSG_ID_CELL_VOLTAGES_1:
            index = 0;
            break;
        case CAN_MSG_ID_CELL_VOLTAGES_2:
            index = 6;
            break;
        case CAN_MSG_ID_CELL_VOLTAGES_3:
            index = 12;
            break;
        default:
            return; // Invalid ID for cell voltages
    }

    for (int i = 0; i < 6; i++) {
        if (index < NOOFVOLTAGETAPS) {
            if (cellData->voltages[i] < MIN_VOLTAGE || cellData->voltages[i] > MAX_VOLTAGE) {
                currentState = SYSTEM_STATE_FAULT;
                return;
            }
            voltageValues.cellVoltages[cellData->segNo][index] = cellData->voltages[i] / 10.0f; // Convert to volts
        }
        voltageValues.lastRxTime = HAL_GetTick();
        index++;
    }
}

void SystemMonitor_UpdateTemperatureData(CAN_MSG_SegTemps_t *tempData)
{
    if (tempData == NULL) return;
    
    int segmentIndex = tempData->segNo;
    
    // Check temperature limits based on mode
    switch (values.mode) {
        case CHARGE:
            if (tempData->maxTemp > tempLimits.charge.max || tempData->minTemp < tempLimits.charge.min) { 
                currentState = SYSTEM_STATE_FAULT;
                return;
            }
            break;
        case DISCHARGE:
            if (tempData->maxTemp > tempLimits.discharge.max || tempData->minTemp < tempLimits.discharge.min) { 
                currentState = SYSTEM_STATE_FAULT;
                return;
            }
            break;
    }
    
    // Store the average temperature for each thermistor in this segment
    // Since CAN message provides aggregated data, we'll distribute the average
    for (int i = 0; i < TEMPERATURES_PER_MODULE; i++) {
        int tempIndex = segmentIndex * TEMPERATURES_PER_MODULE + i;
        if (tempIndex < TOTAL_TEMPERATURES) {
            temperatureValues.cellTemperatures[tempIndex] = tempData->averageTemp / 10.0f; // Convert to °C
        }
    }
    
    temperatureValues.lastRxTime = HAL_GetTick();
}

void SystemMonitor_CalculateTotalPackVoltage(float *packVoltage, SystemVoltageValues_t *voltageValues, BatteryTopology_t *topology)
{
    if (packVoltage == NULL || voltageValues == NULL || topology == NULL) return;

    float totalVoltage = 0.0f;
    for (uint8_t seg = 0; seg < topology->numberOfSegments; seg++) {
        for (uint8_t cell = 0; cell < topology->modulesPerSegment; cell++) {
            totalVoltage += voltageValues->cellVoltages[seg][cell];
        }
    }
    *packVoltage = totalVoltage;
}

void SystemMonitor_SendCAN(void)
{
    CAN_MSG_PackStats_t msg;
    msg.packVoltage_V = (uint16_t)(values.packVoltage);
    msg.packCurrent_A = (int16_t)(values.packCurrent);
    msg.packSOH = (uint8_t)values.packSOH;
    msg.packSOC = (uint8_t)values.packSOC;
    msg.boardTemp = (uint8_t)values.boardTemp;
    msg.SDCstatus = values.SDCstatus ? 1 : 0;

    CAN_Transmit(0x201, (uint8_t *)&msg, sizeof(msg));
}

