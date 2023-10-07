#pragma once

typedef enum {
    CHOICE_NONE = 0,
#define LED_COLOR(_id, _name, _tone) CHOICE_##_name = (1 << _id),
#include "led_color_config.h"
#undef LED_COLOR
} Game_Choice;