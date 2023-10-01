#include "ch32v003fun.h"
#include "pin_mapping_config.h"
#include "ch32v003_GPIO_branchless.h"
#include "buttons.h"

#include <stdio.h>

/*
void EXTI7_0_IRQHandler(void) __attribute__((interrupt));
void EXTI7_0_IRQHandler(void) {
#define GPIO_BUTTON(_name, _pin, _port, _bus)        \
    if(EXTI->INTFR & (1 << _pin)) {                  \
        TuxSays_Buttons_Isr(TuxSays_Button_##_name); \
        EXTI->INTFR |= (1 << _pin);                  \
    }
#include "buttons_config.h"
#undef GPIO_BUTTON
}
void HardFault_Handler(void) __attribute__((interrupt));
void HardFault_Handler(void) {
    while(1) {
        asm volatile("ebreak \n");
    }
}
*/