#pragma once

typedef enum {
    CHOICE_NONE = 0,
#define LED_COLOR(_id, _name, _button_name, _led_name, _tone) CHOICE_##_name = (1 << _id),
#include "led_color_config.h"
#undef LED_COLOR
} _Game_Choices;