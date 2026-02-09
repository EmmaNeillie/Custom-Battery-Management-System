#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "stm32g4xx_hal.h"
#include "stdint.h"
#include <cstdint>

/* Task function pointer type */
typedef void (*TaskFunction_t)(void);

/* Task structure */
typedef struct {
    TaskFunction_t function;    /* Function pointer to the task */
    uint32_t period;           /* Period in milliseconds */
    uint32_t lastRunTime;      /* Last execution time */
    uint8_t isEnabled;         /* Task enabled flag */
} Task_t;

/* Maximum number of tasks */
#define MAX_TASKS 10

/* Scheduler functions */
void Scheduler_Init(void);
void Scheduler_AddTask(TaskFunction_t function, uint32_t period);
void Scheduler_RunTasks(void);

#endif /* SCHEDULER_H */