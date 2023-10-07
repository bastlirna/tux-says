#pragma once

#include "errors.h"
#include <stdint.h>

#include "ch32v003_GPIO_branchless.h"

typedef enum {
#define GPIO_LED(_name, _pin, _port, _initial_state, _bus) TuxSays_Led_##_name,
    TuxSays_Led_None = 0,
#include "leds_config.h"
#undef GPIO_LED
    TuxSays_Led_Count,
} TuxSays_Led;

TuxSays_Error TuxSays_Leds_Init();

TuxSays_Error TuxSays_Leds_Blink(TuxSays_Led led, uint32_t duration);

TuxSays_Error TuxSays_Leds_Set(TuxSays_Led led, enum lowhigh state);

TuxSays_Error TuxSays_Leds_Write(uint32_t state);
