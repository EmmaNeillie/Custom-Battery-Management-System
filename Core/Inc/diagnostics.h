#ifndef DIAGNOSTICS_H
#define DIAGNOSTICS_H

#include "main.h"

void Diagnostics_Init(void);
void Diagnostics_PerformCheck(void);
void Diagnostics_LogError(uint8_t status_code);

#endif /* DIAGNOSTICS_H */