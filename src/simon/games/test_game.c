#include "../logger.h"
#include "../game.h"
#include "../buttons.h"
#include "../leds.h"
#include "../systick.h"
#include "../tone.h"

#include "game_utils.h"

TS_LOG_TAG("G_TST");

typedef enum {
    Blink_State = 0,
    RepeatButton_State,
} Game_State;

static uint32_t last_activity_time;
static Game_State game_state;

TuxSays_Error TuxSays_Game_test_start() {
    last_activity_time = TuxSays_SysTick_Milis();
    game_state = Blink_State;
    return TuxSays_Ok;
}

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

TuxSays_Error TuxSays_Game_test_loop() {
    switch(game_state) {
    case Blink_State:
        TuxSays_Game_toner(CHOICE_YELLOW, 200);
        TuxSays_SysTick_Delay(200);
        TuxSays_Game_toner(CHOICE_BLUE, 200);
        TuxSays_SysTick_Delay(200);
        TuxSays_Game_toner(CHOICE_RED, 200);
        TuxSays_SysTick_Delay(200);
        TuxSays_Game_toner(CHOICE_GREEN, 200);
        TuxSays_SysTick_Delay(200);
        last_activity_time = TuxSays_SysTick_Milis();
        game_state = RepeatButton_State;
        break;
    case RepeatButton_State:
        if((TuxSays_SysTick_Milis() - last_activity_time) > 4000) {
            TS_LOG_I("Exit test due to inactivity");
            TuxSays_Game_Start(&game_attract);
        }
        repeat_button();
        break;
    }

    return TuxSays_Ok;
}

TuxSays_Error TuxSays_Game_test_stop() {
    return TuxSays_Ok;
}