#ifndef SOCFLASH_H
#define SOCFLASH_H

#include <stdint.h>
#include <stdbool.h>
#include "soc.h"

// Call at init. If it returns true, out_state contains restored values.
bool EEPROM_SOC_Persist_Init(socState_t *out_state);

// Call periodically in System Monitor. Saves only when needed.
void EEPROM_SOC_Persist_SaveIfNeeded(const socState_t *state, uint32_t now_ms);

// Call on shutdown / critical events to save immediately.
bool EEPROM_SOC_Persist_ForceSave(const socState_t *state, uint32_t now_ms);

// Wipe stored data (erases reserved page)
bool EEPROM_SOC_Persist_EraseAll(void);

#endif