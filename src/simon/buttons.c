#include "buttons.h"
#include "leds.h"

#include "systick.h"
#include "tasks.h"
#include "ch32v003fun.h"
#include "ch32v003fun_missing.h"
#include "pin_mapping_config.h"
#include "ch32v003_GPIO_branchless.h"

#include <stdio.h>

TS_LOG_TAG("Btns");

static volatile TuxSays_Task_Handle* waiting_task;
static volatile uint32_t isr_state;
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

static inline uint32_t read_state() {
    uint32_t state = 0;
#define GPIO_BUTTON(_name, _pin, _port, _bus)              \
    if((GPIOPortByBase(_port)->INDR & (1 << _pin)) == 0) { \
        uint32_t num = TuxSays_Button_##_name - 1;         \
        state |= (1 << num);                               \
    }
#include "buttons_config.h"
#undef GPIO_BUTTON
    return state;
}

static void init_isr() {
#define GPIO_BUTTON(_name, _pin, _port, _bus) \
    AFIO->EXTICR &= ~(0x03 << (_pin * 2));    \
    AFIO->EXTICR |= (_port << (_pin * 2));    \
    EXTI->INTENR &= ~(1 << (_pin));           \
    EXTI->RTENR |= 1 << (_pin);
#include "buttons_config.h"
#undef GPIO_BUTTON
}

static inline void disable_isr() {
#define GPIO_BUTTON(_name, _pin, _port, _bus) EXTI->INTENR &= ~(1 << (_pin));
#include "buttons_config.h"
#undef GPIO_BUTTON
}

static inline void enable_isr() {
#define GPIO_BUTTON(_name, _pin, _port, _bus) EXTI->INTENR |= 1 << (_pin);
#include "buttons_config.h"
#undef GPIO_BUTTON
}

void TuxSays_Buttons_Isr(uint32_t intfr) {
    disable_isr();
    isr_state = 0;
#define GPIO_BUTTON(_name, _pin, _port, _bus)      \
    if(intfr & (1 << (_pin))) {                    \
        uint32_t num = TuxSays_Button_##_name - 1; \
        isr_state |= (1 << num);                   \
    }
#include "buttons_config.h"
#undef GPIO_BUTTON
    TuxSays_Task_WakeTask((TuxSays_Task_Handle*)waiting_task);
}

TuxSays_Error TuxSays_Buttons_Init() {
    RCC->APB2PCENR |= RCC_APB2Periph_AFIO;

    waiting_task = NULL;

    init_gpio();
    NVIC_EnableIRQ(EXTI7_0_IRQn);

    init_isr();

    return TuxSays_Ok;
}

enum lowhigh TuxSays_Buttons_Get(TuxSays_Button button) {
    uint32_t state = read_state();
    return state & (button - 1) ? high : low;
}

TuxSays_Error TuxSays_Buttons_Read(uint32_t* result, uint32_t timeout) {
    uint32_t state = read_state();
    *result = 0;

    if(state == 0 && timeout != 0) {
        if(waiting_task != NULL) {
            return TuxSays_Error_ResourceBusy;
        }

        waiting_task = TuxSays_Task_GetCurrentTask();
        enable_isr();
        TuxSays_Error err = TuxSays_Task_WaitForEvent(timeout);
        waiting_task = NULL;

        if(err != TuxSays_Ok) {
            // eg.: timer timeout
            return err;
        }

        state = isr_state | read_state();
    }

    *result = state;

    return TuxSays_Ok;
}

/*
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
*/