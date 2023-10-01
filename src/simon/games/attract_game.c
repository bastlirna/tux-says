#include "../game.h"
#include "../buttons.h"
#include "../leds.h"
#include "../systick.h"
#include "../tone.h"

#include "game_utils.h"

#define BLINK_PERIOD 100

typedef enum {
    State_init = 0,
#define LED_COLOR(_id, _name, _button_name, _led_name, _tone) State_##_id,
#include "led_color_config.h"
    State_count
#undef LED_COLOR
} Game_State;

volatile static Game_State current_state = 0;
volatile static uint32_t last_run = 0;

TuxSays_Error TuxSays_Game_attract_start() {
    current_state = State_init;
    last_run = 0;

    return TuxSays_Ok;
}

TuxSays_Error TuxSays_Game_attract_loop() {
    if((TuxSays_SysTick_Milis() - last_run) > BLINK_PERIOD) {
        last_run = TuxSays_SysTick_Milis();

        if(current_state == State_init) {
            current_state++; // counting hack (enum starts with 0, but first state have 1)
        }

        //return TuxSays_Ok;
        if(TuxSays_Game_checkButton() != TuxSays_Button_None) {
            TuxSays_Game_Start(&game_test);
            return TuxSays_Ok;
        }

        // blink led related to current state
        switch(current_state) {
#define LED_COLOR(_id, _name, _button_name, _led_name, _tone) \
    case State_##_id:                                         \
        TuxSays_Game_setLEDs(CHOICE_##_name);                 \
        break;
#include "led_color_config.h"
#undef LED_COLOR
        default:
            break;
        }

        // move to next state
        current_state = (current_state + 1) % State_count;
    }
    return TuxSays_Ok;
}

TuxSays_Error TuxSays_Game_attract_stop() {
    return TuxSays_Ok;
}