#include "systick.h"

#include "ch32v003fun.h"
#include "ch32v003fun_missing.h"

static volatile uint32_t millis;

void SysTick_Handler(void) __attribute__((interrupt));
void SysTick_Handler(void) {
    SysTick->SR = 0; // clear IRQ
    millis++;
}

void TuxSays_SysTick_Init() {
    // disable default SysTick behavior
    SysTick->CTLR = 0;

    // enable the SysTick IRQ
    NVIC_EnableIRQ(SysTicK_IRQn);

    // Start at zero
    SysTick->CNT = 0;
    millis = 0;

    // Enable SysTick counter, IRQ, HCLK/1
    SysTick->CTLR = SYSTICK_CTLR_STE | SYSTICK_CTLR_STIE | SYSTICK_CTLR_STRE | SYSTICK_CTLR_STCLK;

    uint32_t systick_prescaler = 1;

    if((SysTick->CTLR & SYSTICK_CTLR_STCLK) == 0) {
        systick_prescaler = 8;
    }

    // TODO calc HPRE prescaller from RCC->CFGR0 HPRE3-0 bits

    // Set the tick interval to 1ms for normal op
    SysTick->CMP = (FUNCONF_SYSTEM_CORE_CLOCK / systick_prescaler / (1000)) - 1;
}

void TuxSays_SysTick_Delay(uint32_t ms) {
    uint32_t start = millis;
    while(millis - start < ms)
        ;
}

uint32_t TuxSays_SysTick_Milis() {
    return millis;
}
