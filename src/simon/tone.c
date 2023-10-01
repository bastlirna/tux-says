#include "tone.h"
#include "pin_mapping_config.h"
#include "ch32v003fun.h"
#include "ch32v003fun_missing.h"
#include "ch32v003_GPIO_branchless.h"
#include "systick.h"
#include <stdio.h>

#define VOLUME 5

TuxSays_Error TuxSays_Tone_Init() {
    /// Enable GPIOC and TIM1
    RCC->APB2PCENR |= RCC_APB2Periph_GPIOC | RCC_APB2Periph_TIM1;

    // Reset TIM1 to init all regs
    RCC->APB2PRSTR |= RCC_APB2Periph_TIM1;
    RCC->APB2PRSTR &= ~RCC_APB2Periph_TIM1;

    GPIO_pinMode(
        GPIOv_from_PORT_PIN(GPIO_BEEPER_PORT, GPIO_BEEPER_PIN),
        GPIO_pinMode_O_pushPullMux,
        GPIO_Speed_2MHz);
    GPIO_tim1_map(GPIO_tim1_output_set_1__C6_C7_C0_D3__C3_C4_D1);

    // Prescaler
    TIM1->PSC = 0x01;

    // Auto Reload - sets period
    TIM1->ATRLR = 65530;

    // Reload immediately
    TIM1->SWEVGR |= TIM_UG;

    // Enable CH2 output, positive pol
    TIM1->CCER |= TIM_CC2E | TIM_CC2P;

    // CH2 Mode is output, PWM1 (CC2S = 00, OC2M = 110)
    TIM1->CHCTLR1 |= TIM_OC2M_2 | TIM_OC2M_1;

    // Set the Capture Compare Register value to 50% initially
    TIM1->CH2CVR = 100;

    // Enable TIM1 outputs
    //TIM1->BDTR |= TIM_MOE;

    // Enable TIM1
    //TIM1->CTLR1 |= TIM_CEN;

    return TuxSays_Ok;
}

static void enable(uint32_t frequency) {
    //VOLUME

    uint32_t presc = 16;
    uint32_t atrlr = FUNCONF_SYSTEM_CORE_CLOCK / frequency / presc;

    while(atrlr > 65000) {
        presc *= 2;
        atrlr = FUNCONF_SYSTEM_CORE_CLOCK / frequency / presc;
    }

    //atrlr = 65530;

    TIM1->CH2CVR = (atrlr * VOLUME) / 1000;
    TIM1->PSC = presc;
    TIM1->ATRLR = atrlr;

    // Enable TIM1 outputs
    TIM1->BDTR |= TIM_MOE;

    // Enable TIM1
    TIM1->CTLR1 |= TIM_CEN;
}

static void disable() {
    // Enable TIM1 outputs
    TIM1->BDTR &= ~TIM_MOE;

    // Enable TIM1
    TIM1->CTLR1 &= ~TIM_CEN;
}

TuxSays_Error TuxSays_Tone(uint32_t frequency, uint32_t duration) {
    if(frequency > 0) {
        enable(frequency);
    }

    TuxSays_SysTick_Delay(200);
    disable();

    return TuxSays_Ok;
}
