#pragma once

#include <stdint.h>
#include "errors.h"

//typedef struct _TuxSays_Task TuxSays_Task;

typedef enum {
    TuxSays_Task_None = 0,
#define TASK(_name, _fn, _priority, _stack_size) TuxSays_Task##_name,
#include "task_list_config.h"
#undef TASK
    TuxSays_Task_Count,
} TuxSays_Task;

typedef struct TuxSays_Task_Handle TuxSays_Task_Handle;

#define TS_TASK_WAIT_FOREVER ((uint32_t)-1)

#define TS_TASK_NO_BLOCKING ((uint32_t)0)

TuxSays_Error TuxSays_Task_Init();

TuxSays_Error TuxSays_Task_StartScheduler();

TuxSays_Error TuxSays_Task_Yield();

TuxSays_Error TuxSays_Task_Delay(uint32_t ms);

TuxSays_Error TuxSays_Task_DumpStackInfo();

TuxSays_Error TuxSays_Task_CalculateSleepTime(uint32_t* ms);

TuxSays_Task_Handle* TuxSays_Task_GetCurrentTask();

TuxSays_Error TuxSays_Task_WaitForEvent(uint32_t ms);

TuxSays_Error TuxSays_Task_WakeTask(const TuxSays_Task_Handle* handle);