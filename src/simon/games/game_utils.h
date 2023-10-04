#pragma once

#include "ch32v003fun.h"
#include "ch32v003_GPIO_branchless.h"
#include "../errors.h"
#include "../buttons.h"
#include <stdint.h>

//#include "game_choices.h"

//typedef

typedef enum {
    TuxSays_Game_PlayerLose = 0,
    TuxSays_Game_PlayerWin,
} TuxSays_Game_Result;

void TuxSays_Game_setLEDs(uint32_t choice);

TuxSays_Error TuxSays_Game_toner(uint32_t choice, uint32_t buzz_length_ms);

TuxSays_Error TuxSays_Game_wait_for_button(uint32_t* result, uint32_t timeout);

TuxSays_Button TuxSays_Game_checkButton();

TuxSays_Error TuxSays_Game_play_winner();

TuxSays_Error TuxSays_Game_play_loser();