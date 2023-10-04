#pragma once

#include "ch32v003fun.h"
#include "ch32v003_GPIO_branchless.h"
#include "errors.h"
#include <stdint.h>

typedef struct {
    TuxSays_Error (*start)();
    TuxSays_Error (*loop)();
    TuxSays_Error (*stop)();
    const char* const name;
} TuxSays_Game;

TuxSays_Error TuxSays_Game_Start(const TuxSays_Game* game);

TuxSays_Error TuxSays_Game_Stop();

TuxSays_Error TuxSays_Game_Loop();

#define GameById(_code) (&game_##_code)

#define GAME(_name, _code) extern const TuxSays_Game game_##_code;
#include "game_list_config.h"
#undef GAME