#include "buttons.h"
#include "leds.h"

#include "systick.h"
#include "ch32v003fun.h"
#include "ch32v003fun_missing.h"
#include "pin_mapping_config.h"
#include "ch32v003_GPIO_branchless.h"

#include <stdio.h>

TS_LOG_TAG("Btns");

//static volatile uint32_t _button_irq_state;

//    EXTI->FTENR |= EXTI_FTENR_TR0 << _pin;
//    EXTI->INTENR |= EXTI_INTENR_MR0 << _pin;
//    AFIO->EXTICR |= (_port << (_pin * 2));

static void init_gpio() {
#define GPIO_BUTTON(_name, _pin, _port, _bus) \
    RCC->APB2PCENR |= _bus;                   \
    GPIO_pinMode(GPIOv_from_PORT_PIN(_port, _pin), GPIO_pinMode_I_pullUp, GPIO_Speed_2MHz);
#include "buttons_config.h"
#undef GPIO_BUTTON
}
/*
void TuxSays_Buttons_Isr(TuxSays_Button button) {
    switch(button) {
#define GPIO_BUTTON(_name, _pin, _port, _bus)             \
    case TuxSays_Button_##_name:                          \
        if(GPIOPortByBase(_port)->INDR & (1 << button)) { \
            _button_irq_state &= ~(1 << button);          \
        } else {                                          \
            _button_irq_state |= (1 << button);           \
        }
        break;
#include "buttons_config.h"
#undef GPIO_BUTTON
    default:
    }
}
*/

TuxSays_Error TuxSays_Buttons_Init() {
    RCC->APB2PCENR |= RCC_APB2Periph_AFIO;

    init_gpio();
    NVIC_EnableIRQ(EXTI7_0_IRQn);

    // Configure the IO as an interrupt.
    //AFIO->EXTICR = 3 << (3 * 2); //PORTD.3 (3 out front says PORTD, 3 in back says 3)
    //EXTI->INTENR = 1 << 3; // Enable EXT3
    //EXTI->RTENR = 1 << 3; // Rising edge trigger

    // enable interrupt

    return TuxSays_Ok;
}

enum lowhigh TuxSays_Buttons_Get(TuxSays_Button button) {
    switch(button) {
#define GPIO_BUTTON(_name, _pin, _port, _bus)                  \
    case TuxSays_Button_##_name:                               \
        if((GPIOPortByBase(_port)->INDR & (1 << _pin)) == 0) { \
            return high;                                       \
        }                                                      \
        break;
#include "buttons_config.h"
#undef GPIO_BUTTON
    default:
        break;
    }
    return low;
}

TuxSays_Error TuxSays_Buttons_Loop() {
    static uint32_t last_run = 0;
    //_button_irq_state

    if((TuxSays_SysTick_Milis() - last_run) > 100) {
#define GPIO_BUTTON(_name, _pin, _port, _bus)                      \
    if(!(GPIOPortByBase(_port)->INDR & (1 << _pin))) {             \
        TuxSays_Leds_Blink((uint32_t)TuxSays_Button_##_name, 200); \
    }
#include "buttons_config.h"
#undef GPIO_BUTTON

        last_run = TuxSays_SysTick_Milis();
    }

    return TuxSays_Ok;
}
