#include "ch32v003fun.h"
#include "pin_mapping_config.h"
#include "ch32v003_GPIO_branchless.h"
#include "buttons.h"
#include "logger.h"
#include <stdio.h>

TS_LOG_TAG("ISR");
void EXTI7_0_IRQHandler(void) __attribute__((interrupt));
void EXTI7_0_IRQHandler(void) {
    uint32_t ifrmask = 0;
#define GPIO_BUTTON(_name, _pin, _port, _bus) ifrmask |= (1 << _pin);
#include "buttons_config.h"
#undef GPIO_BUTTON

    uint32_t old_intfr = EXTI->INTFR;
    if(EXTI->INTFR & ifrmask) {
        EXTI->INTFR |= ifrmask;
        TuxSays_Buttons_Isr(old_intfr);
    }
}

void HardFault_Handler(void) __attribute__((naked));
void HardFault_Handler(void) {
    // reset system
    //PFIC->SCTLR |= PFIC_SCTLR_SYSRESET;

    asm volatile("csrr a0, mcause \n"
                 "csrr a1, mepc \n"
                 "loop: \n"
                 "j loop");
}
