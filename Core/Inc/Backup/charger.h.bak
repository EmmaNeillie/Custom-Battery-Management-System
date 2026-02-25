#ifndef CHARGER_H
#define CHARGER_H

#include <stdint.h>

#define CHARGER_MSG_INTERVAL_MS 1000
#define CHARGER_MSG1_ID 0x1806E5F4 // From BMS to Charger
#define CHARGER_MSG2_ID 0x18FF50E5 // From Charger to BMS

typedef struct {
    uint8_t MaxChargeVoltageH;
    uint8_t MaxChargeVoltageL;
    uint8_t MaxChargeCurrentH;
    uint8_t MaxChargeCurrentL;
    uint8_t Control;
    uint8_t Reserved[3];
} ChargerMsg1;

typedef struct {
    uint8_t OutputVoltageH;
    uint8_t OutputVoltageL;
    uint8_t OutputCurrentH;
    uint8_t OutputCurrentL;
    uint8_t Status;         
    uint8_t Reserved[3];
} ChargerMsg2;

typedef struct {
    uint8_t HardwareFailure:    1;
    uint8_t Temperature:        1;
    uint8_t InputVoltage:       1;
    uint8_t StartingState:      1;
    uint8_t CommsState:         1;
    uint8_t Reserved:           3;
} ChargerStatus;

/* Operation Mode 

1. The BMS sends operating information (ChargerMsg1) to the charger at a fixed interval of 1s. After receiving this message, the charger will work under
the Voltage and Current in Message. If the Message is not received within 5s, it will enter into communication error state and stop charging.

2. The charger sends broadcast messages (ChargerMsg2) at intervals of 1s. The dispaly meter can show the status of the charger according to up to date
information.

*/

void CheckChargerConnection(void);

#endif // CHARGER_H