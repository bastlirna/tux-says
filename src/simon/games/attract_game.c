#include "../game.h"
#include "../buttons.h"
#include "../leds.h"
#include "../systick.h"
#include "../tone.h"
#include "../tasks.h"

#include "../ch32v003fun_missing.h"
#include "game_choices.h"
#include "game_utils.h"

#define BLINK_PERIOD 100

static struct {
    enum {
        FastMode = 0,
        SleepMode,
    } status;
    uint32_t seed;
    uint32_t started_at;
} game;

TS_LOG_TAG("G_Atrct");

TuxSays_Error TuxSays_Game_attract_start() {
    game.seed = ESIG->UID0 & ESIG->UID1 & ESIG->UID2 & TuxSays_SysTick_Milis();
    game.started_at = TuxSays_SysTick_Milis();
    game.status = FastMode;
    return TuxSays_Ok;
}

static TuxSays_Error sound_disable() {
    CHECK_ERR(TuxSays_Tone(2217, 200));
    CHECK_ERR(TuxSays_Task_Delay(100));
    CHECK_ERR(TuxSays_Tone(1750, 200));
    CHECK_ERR(TuxSays_Tone_SetSilentMode(1));

    return TuxSays_Ok;
}

static TuxSays_Error sound_enable() {
    CHECK_ERR(TuxSays_Tone_SetSilentMode(0));
    CHECK_ERR(TuxSays_Tone(1750, 200));
    CHECK_ERR(TuxSays_Task_Delay(100));
    CHECK_ERR(TuxSays_Tone(2217, 200));

    return TuxSays_Ok;
}

static uint32_t sound_on_off() {
    uint32_t tm = TuxSays_SysTick_Milis();
    uint32_t active_buttons;

    do {
        // wait for button release
        TuxSays_Buttons_Read(&active_buttons, TS_TASK_NO_BLOCKING);
        TuxSays_Leds_Write(active_buttons);
        TuxSays_Task_Delay(10);

        if((TuxSays_SysTick_Milis() - tm) > 3000) {
            // toggle sound

            if(active_buttons == CHOICE_GREEN) {
                sound_disable();
            } else if(active_buttons == CHOICE_RED) {
                sound_enable();
            }

            return 1;
        }

    } while(active_buttons != 0);

    return 0;
}

TuxSays_Error TuxSays_Game_attract_loop() {
    game.seed &= TuxSays_SysTick_Milis();

    if(game.status == FastMode && (TuxSays_SysTick_Milis() - game.started_at) > 10000) {
        TS_LOG_D("goto sleep mode");
        game.status = SleepMode;
    }

    for(uint32_t i = 0; i < 4; i++) {
        TuxSays_Leds_Write(1 << i);

        uint32_t result;
        TuxSays_Buttons_Read(&result, 100);
        if(result != CHOICE_NONE && !sound_on_off()) {
            TS_LOG_D("start test (button = %08X)", result);
            TuxSays_Game_Start(&game_memory);
            return TuxSays_Ok;
        }
    }

    if(game.status == SleepMode) {
        TuxSays_Leds_Write(0);

        uint32_t sleep_delay = 60000 + (((uint32_t)preudo_rand_r(&game.seed)) % 60000);
        uint32_t active_buttons;
        TuxSays_Buttons_Read(&active_buttons, sleep_delay);
        TS_LOG_D("wakey wakey");

        if(active_buttons != 0) {
            TuxSays_Leds_Write(0x0f);
            TuxSays_Task_Delay(100);

            TuxSays_Leds_Write(0);
            TuxSays_Task_Delay(100);

            game.started_at = TuxSays_SysTick_Milis();
            game.status = FastMode;
        }
    }

    return TuxSays_Ok;
}

TuxSays_Error TuxSays_Game_attract_stop() {
    TuxSays_Leds_Write(0);
    return TuxSays_Ok;
}