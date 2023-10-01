#pragma once

#include "ch32v003fun.h"
#include "ch32v003_GPIO_branchless.h"
#include "../errors.h"
#include <stdint.h>

typedef enum {
    TuxSays_Game_PlayerLose = 0,
    TuxSays_Game_PlayerWin,
} TuxSays_Game_Result;

TuxSays_Error TuxSays_Game_Init();

TuxSays_Error TuxSays_Game_Loop();