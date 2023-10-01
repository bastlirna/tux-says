#pragma once

#include "ch32v003fun.h"
#include "ch32v003_GPIO_branchless.h"
#include "../errors.h"
#include "../buttons.h"
#include <stdint.h>

#include "game_choices.h"

void TuxSays_Game_setLEDs(uint32_t choice);

void TuxSays_Game_toner(_Game_Choices choice, uint32_t buzz_length_ms);

TuxSays_Button TuxSays_Game_checkButton();