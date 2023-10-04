#include <stddef.h>
#include "game.h"
#include "errors.h"
#include "systick.h"
#include "leds.h"

TS_LOG_TAG("GM");

#define GAME(_name, _code)                        \
    TuxSays_Error TuxSays_Game_##_code##_start(); \
    TuxSays_Error TuxSays_Game_##_code##_loop();  \
    TuxSays_Error TuxSays_Game_##_code##_stop();  \
    const TuxSays_Game game_##_code = {           \
        .name = #_name,                           \
        .start = TuxSays_Game_##_code##_start,    \
        .loop = TuxSays_Game_##_code##_loop,      \
        .stop = TuxSays_Game_##_code##_stop,      \
    };
#include "game_list_config.h"
#undef GAME

static volatile TuxSays_Game* current_game = NULL;

static TuxSays_Error stop() {
    if(current_game == NULL) {
        return TuxSays_Error_NoAnyGameRunnig;
    }

    TS_LOG_I("Stopping game %s", current_game->name);
    CHECK_ERR(current_game->stop());
    current_game = NULL;

    return TuxSays_Ok;
}

TuxSays_Error TuxSays_Game_Start(const TuxSays_Game* game) {
    if(current_game != game) {
        TuxSays_Error err = stop();
        switch(err) {
        case TuxSays_Error_NoAnyGameRunnig: // correct state here
            break;
        default:
            CHECK_ERR(err);
        }
    }
    TS_LOG_I("Starting game %s", game->name);

    CHECK_ERR_M(game->start(), "unable to start game '%s'", game->name);
    current_game = (TuxSays_Game*)game;

    return TuxSays_Ok;
}

TuxSays_Error TuxSays_Game_Stop() {
    CHECK_ERR_M(stop(), "unable to stop game '%s'", current_game->name);
    return TuxSays_Ok;
}

TuxSays_Error TuxSays_Game_Loop() {
    if(current_game == NULL) {
        return LOG_ERR(TuxSays_Error_NoAnyGameRunnig);
    }

    CHECK_ERR_M(current_game->loop(), "unable to loop game '%s'", current_game->name);

    return TuxSays_Ok;
}