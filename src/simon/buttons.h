#pragma once

#include "ch32v003fun.h"
#include "ch32v003_GPIO_branchless.h"
#include "errors.h"
#include <stdint.h>

typedef enum {
#define GPIO_BUTTON(_name, _pin, _port, _bus) TuxSays_Button_##_name,
    TuxSays_Button_None = 0,
#include "buttons_config.h"
#undef GPIO_BUTTON
    TuxSays_Button_Count,
} TuxSays_Button;

TuxSays_Error TuxSays_Buttons_Init();

enum lowhigh TuxSays_Buttons_Get(TuxSays_Button button);

void TuxSays_Buttons_Isr(TuxSays_Button button);

TuxSays_Error TuxSays_Buttons_Loop();