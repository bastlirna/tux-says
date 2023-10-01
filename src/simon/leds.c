#include "leds.h"

#include "systick.h"
#include "config.h"
#include "ch32v003fun.h"
#include "ch32v003fun_missing.h"
#include "pin_mapping_config.h"
#include "ch32v003_GPIO_branchless.h"

#include <stdio.h>

TS_LOG_TAG("LEDS");

static void init_gpio() {
#define GPIO_LED(_name, _pin, _port, _initial_state, _bus)                                    \
    RCC->APB2PCENR |= _bus;                                                                   \
    GPIO_pinMode(GPIOv_from_PORT_PIN(_port, _pin), GPIO_pinMode_O_pushPull, GPIO_Speed_2MHz); \
    GPIO_digitalWrite(GPIOv_from_PORT_PIN(_port, _pin), _initial_state);
#include "leds_config.h"
#undef GPIO_LED
}

static inline void wreg(enum GPIO_port_n port, uint8_t pin, enum lowhigh state) {
    if(state == low) {
        pin += 16;
    }
    GPIOPortByBase(port)->BSHR = (1 << (pin));
}

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

TuxSays_Error TuxSays_Leds_Init() {
    init_gpio();

    return TuxSays_Ok;
}

TuxSays_Error TuxSays_Leds_Blink(TuxSays_Led led, uint32_t duration) {
    set_state(led, high);
    //Delay_Ms(duration);
    TuxSays_SysTick_Delay(duration);
    set_state(led, low);

    return TuxSays_Ok;
}

TuxSays_Error TuxSays_Leds_Set(TuxSays_Led led, enum lowhigh state) {
    set_state(led, state);
    return TuxSays_Ok;
}