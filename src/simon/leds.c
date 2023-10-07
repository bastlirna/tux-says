#include "leds.h"

#include "systick.h"
#include "config.h"
#include "ch32v003fun.h"
#include "ch32v003fun_missing.h"
#include "pin_mapping_config.h"
#include "ch32v003_GPIO_branchless.h"

#include <stdio.h>

TS_LOG_TAG("LEDS");

static volatile uint32_t current_state = 0;

static void init_gpio() {
#define GPIO_LED(_name, _pin, _port, _initial_state, _bus)                                    \
    RCC->APB2PCENR |= _bus;                                                                   \
    GPIO_pinMode(GPIOv_from_PORT_PIN(_port, _pin), GPIO_pinMode_O_pushPull, GPIO_Speed_2MHz); \
    GPIO_digitalWrite(GPIOv_from_PORT_PIN(_port, _pin), _initial_state);
#include "leds_config.h"
#undef GPIO_LED
}

static TuxSays_Error set_state(uint32_t state) {
    current_state = state;
#define GPIO_LED(_name, _pin, _port, _initial_state, _bus) \
    if(state & (1 << (TuxSays_Led_##_name - 1))) {         \
        GPIOPortByBase(_port)->BSHR = (1 << (_pin));       \
    } else {                                               \
        GPIOPortByBase(_port)->BSHR = (1 << (_pin + 16));  \
    }
#include "leds_config.h"
#undef GPIO_LED
    return TuxSays_Ok;
}
/*
static TuxSays_Error set_state(TuxSays_Led led, enum lowhigh state) {
    switch(led) {
#define GPIO_LED(_name, _pin, _port, _initial_state, _bus) \
    case TuxSays_Led_##_name:                              \
        wreg(_port, _pin, state);                          \
        break;
#include "leds_config.h"
#undef GPIO_LED
    default:
        return LOG_ERR_M(TuxSays_Error_InvalidArgument, "led %d not defined", led);
    }
    return TuxSays_Ok;
}
*/

TuxSays_Error TuxSays_Leds_Init() {
    init_gpio();

    return TuxSays_Ok;
}

TuxSays_Error TuxSays_Leds_Blink(TuxSays_Led led, uint32_t duration) {
    if(led == TuxSays_Led_None) {
        return TuxSays_Error_InvalidArgument;
    }

    current_state |= (1 << (led - 1));
    set_state(current_state);

    TuxSays_SysTick_Delay(duration);

    current_state &= ~(1 << (led - 1));
    set_state(current_state);

    return TuxSays_Ok;
}

TuxSays_Error TuxSays_Leds_Write(uint32_t state) {
    set_state(state);

    return TuxSays_Ok;
}

TuxSays_Error TuxSays_Leds_Set(TuxSays_Led led, enum lowhigh state) {
    if(led == TuxSays_Led_None) {
        set_state(0);
        return TuxSays_Ok;
    }

    current_state &= ~(1 << (led - 1));
    if(state == high) {
        current_state |= (1 << (led - 1));
    }

    set_state(current_state);

    return TuxSays_Ok;
}