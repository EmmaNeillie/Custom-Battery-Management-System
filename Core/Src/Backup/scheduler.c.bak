#include "scheduler.h"

/* Task list */
static Task_t tasks[MAX_TASKS];
static uint8_t taskCount = 0;

/* Initialize the scheduler */
void Scheduler_Init(void)
{
    taskCount = 0;
    for (uint8_t i = 0; i < MAX_TASKS; i++) {
        tasks[i].function = NULL;
        tasks[i].period = 0;
        tasks[i].lastRunTime = 0;
        tasks[i].isEnabled = 0;
    }
}

/* Add a task to the scheduler */
void Scheduler_AddTask(TaskFunction_t function, uint32_t period)
{
    if (taskCount < MAX_TASKS && function != NULL) {
        tasks[taskCount].function = function;
        tasks[taskCount].period = period;
        tasks[taskCount].lastRunTime = HAL_GetTick();
        tasks[taskCount].isEnabled = 1;
        taskCount++;
    }
}

/* Run scheduled tasks */
void Scheduler_RunTasks(void)
{
    uint32_t currentTime = HAL_GetTick();
    
    for (uint8_t i = 0; i < taskCount; i++) {
        if (tasks[i].isEnabled && tasks[i].function != NULL) {
            if ((currentTime - tasks[i].lastRunTime) >= tasks[i].period) {
                tasks[i].function();
                tasks[i].lastRunTime = currentTime;
            }
        }
    }
}