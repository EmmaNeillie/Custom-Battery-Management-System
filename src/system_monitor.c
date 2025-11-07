#include "system_monitor.h"
#include "adc.h"
#include "utilities.h"
#include "project.h"
#include <string.h>

/* Simple system monitor implementation
 * - Maintains a small set of computed values (voltage, current, power)
 * - Provides a CAN message handler that parses selected message IDs
 * - Reads current from ADC when needed via CurrentRead()
 *
 * Notes / assumptions:
 * - CAN messages carrying pack voltage use StdId 0x100 and 2-byte millivolt value
 * - ADC/CurrentRead() returns a filtered amperage in amps
 * - This is intentionally lightweight; adapt IDs/format to your actual CAN spec
 */

void SystemMonitor_Init(void)
{
    memset(&values, 0, sizeof(values));
    values.packVoltage = 0.0f;
    values.packCurrent = 0.0f;
    values.packPower = 0.0f;
    values.lastRxTime = 0;

    currentState = SYSTEM_STATE_NORMAL;
}

void SystemMonitor_Update(void)
{
    /* Periodic update called by scheduler. We'll refresh the current reading
     * and recompute power from the most recent voltage reading. */
    float current = CurrentRead();
    values.packCurrent = current;

    /* Recompute power using latest known voltage */
    values.packPower = values.packVoltage * values.packCurrent;

    /* Example simple state transition: if current sensor indicates a large draw,
       we could set a fault state (thresholds are placeholders). */
    if (values.packCurrent > 50.0f) {
        currentState = SYSTEM_STATE_FAULT;
    }
}

void SystemMonitor_HandleCAN(CAN_RxHeaderTypeDef *rxHeader, uint8_t *data)
{
    if (rxHeader == NULL || data == NULL) return;

    /* Parse messages based on standard ID. Update internal values accordingly. */
    switch (rxHeader->StdId) {
        case 0x100: {
            /* Pack voltage message: first two bytes = millivolts (big-endian) */
            if (rxHeader->DLC >= 2) {
                uint16_t raw = ((uint16_t)data[0] << 8) | (uint16_t)data[1];
                values.packVoltage = ((float)raw) / 1000.0f; /* mV -> V */
                values.lastRxTime = HAL_GetTick();
                /* Recompute power with most recent current reading */
                values.packPower = values.packVoltage * values.packCurrent;
            }
            break;
        }

        case 0x101: {
            /* Example: a cell-level report or balancing command. Placeholder parsing. */
            /* We'll just record that we received something and update timestamp */
            values.lastRxTime = HAL_GetTick();
            break;
        }

        default:
            /* Unknown/unused ID for now */
            break;
    }
}

void SystemMonitor_GetValues(SystemMonitorValues_t *outValues)
{
    if (outValues == NULL) return;
    memcpy(outValues, &values, sizeof(values));
}

