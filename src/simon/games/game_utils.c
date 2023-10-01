#include "game_utils.h"

#include "../game.h"
#include "../buttons.h"
#include "../leds.h"
#include "../systick.h"
#include "../tone.h"

void TuxSays_Game_setLEDs(uint32_t choice) {
#define LED_COLOR(_id, _name, _button_name, _led_name, _tone) \
    if((choice & CHOICE_##_name) != 0) {                      \
        TuxSays_Leds_Set(TuxSays_Led_##_led_name, high);      \
    } else {                                                  \
        TuxSays_Leds_Set(TuxSays_Led_##_led_name, low);       \
    }
#include "led_color_config.h"
#undef LED_COLOR
}

// Light an LED and play tone
// Red, upper left:     440Hz - 2.272ms - 1.136ms pulse
// Green, upper right:  880Hz - 1.136ms - 0.568ms pulse
// Blue, lower left:    587.33Hz - 1.702ms - 0.851ms pulse
// Yellow, lower right: 784Hz - 1.276ms - 0.638ms pulse
void TuxSays_Game_toner(_Game_Choices choice, uint32_t buzz_length_ms) {
    TuxSays_Game_setLEDs(choice); //Turn on a given LED

    //Play the sound associated with the given LED
    switch(choice) {
#define LED_COLOR(_id, _name, _button_name, _led_name, _tone) \
    case CHOICE_##_name:                                      \
        TuxSays_Tone(_tone, buzz_length_ms);                  \
        break;
#include "led_color_config.h"
#undef LED_COLOR
    default:
        break;
    }
    TuxSays_Game_setLEDs(CHOICE_NONE); // Turn off all LEDs
}

TuxSays_Button TuxSays_Game_checkButton() {
#define GPIO_BUTTON(_name, _pin, _port, _bus)                 \
    if(TuxSays_Buttons_Get(TuxSays_Button_##_name) == high) { \
        return TuxSays_Button_##_name;                        \
    }
#include "buttons_config.h"
#undef GPIO_BUTTON

    return TuxSays_Button_None;
}
