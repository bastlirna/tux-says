#include "../logger.h"
#include "../game.h"
#include "../buttons.h"
#include "../leds.h"
#include "../systick.h"
#include "../tone.h"
#include "../tasks.h"
#include "game_choices.h"

#include "game_utils.h"

TS_LOG_TAG("G_Test");

TuxSays_Error TuxSays_Game_test_start() {
    return TuxSays_Ok;
}

/*
static void repeat_button() {
    TuxSays_Button btn = TuxSays_Game_checkButton();
    switch(btn) {
#define LED_COLOR(_id, _name, _button_name, _led_name, _tone) \
    case TuxSays_Button_##_button_name:                       \
        TuxSays_Game_toner(CHOICE_##_name, 200);              \
        TS_LOG_D("CHOICE_" #_name);                           \
        last_activity_time = TuxSays_SysTick_Milis();         \
        break;
#include "led_color_config.h"
#undef LED_COLOR
    default:
        break;
    }
}
*/
TuxSays_Error TuxSays_Game_test_loop() {
    TuxSays_Game_toner(CHOICE_YELLOW, 200);
    TuxSays_Task_Delay(200);
    TuxSays_Game_toner(CHOICE_BLUE, 200);
    TuxSays_Task_Delay(200);
    TuxSays_Game_toner(CHOICE_RED, 200);
    TuxSays_Task_Delay(200);
    TuxSays_Game_toner(CHOICE_GREEN, 200);
    TuxSays_Task_Delay(200);

    uint32_t ms = TuxSays_SysTick_Milis();
    while(1) {
        uint32_t result;
        TuxSays_Buttons_Read(&result, 20);

        if(result != 0) {
            TS_LOG_I("button state = %08X", result);

            TuxSays_Game_toner(result, 200);

            do {
                // wait for button release
                TuxSays_Buttons_Read(&result, TS_TASK_NO_BLOCKING);
                TuxSays_Task_Delay(10);
            } while(result != 0);

            ms = TuxSays_SysTick_Milis();
        }

        if((TuxSays_SysTick_Milis() - ms) > 4000) {
            TS_LOG_I("Exit test due to inactivity");
            TuxSays_Game_Start(&game_attract);
            return TuxSays_Ok;
        }

        //TuxSays_Game_wait_for_button(&result, 4000);
        //if(result == CHOICE_NONE) {
        //    TS_LOG_I("Exit test due to inactivity");
        //    TuxSays_Game_Start(&game_attract);
        //    return TuxSays_Ok;
        //}

        //
        //TuxSays_Task_Delay(200);
    }

    return TuxSays_Ok;
}

TuxSays_Error TuxSays_Game_test_stop() {
    TuxSays_Leds_Write(0);
    return TuxSays_Ok;
}