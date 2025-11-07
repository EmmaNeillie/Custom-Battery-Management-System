#include "can.h"
#include "project.h"
#include "system_monitor.h"


void CAN_Init(void){
    // Configure CAN1 & CAN2: bitrates, filters, enable interrupts etc.
}

void CAN_Transmit(uint32_t id, uint8_t *data, uint8_t len){
    // Transmit CAN message with given ID and data
}

/* This callback should be invoked by the low-level CAN IRQ/driver when a
 * message is received. Forward the message to the system monitor for
 * processing. Keep this function small so it can be used in IRQ context.
 */
void CAN_RxCallback(CAN_RxHeaderTypeDef *rxHeader, uint8_t *data){
    if (rxHeader == NULL || data == NULL) return;

    /* Forward to system monitor which will do parsing and calculations. */
    SystemMonitor_HandleCAN(rxHeader, data);
}

void CAN_ErrorCallback(void){
    // User-defined callback for CAN errors
}

void CAN_BusOffRecovery(void){
    // User-defined function to recover from bus-off state
}
