#include "tone.h"
#include "pin_mapping_config.h"
#include "ch32v003fun.h"
#include "ch32v003fun_missing.h"
#include "ch32v003_GPIO_branchless.h"
#include "systick.h"
#include "logger.h"
#include "tasks.h"
#include "power_management.h"
#include <stdio.h>

TS_LOG_TAG("Tone");

static inline void disable();
static inline void enable(uint32_t frequency);

static struct {
    volatile enum {
        Tone_Off = 0,
        Tone_Play,
        Tone_PlayGenerator,
    } state;
    struct {
        volatile TuxSays_Tone_Generator cb;
        volatile void* ctx;
        volatile TuxSays_Task_Handle* waiting_task;
        volatile uint32_t next_freq;
    } generator;
    volatile enum {
        SilentMode_Off = 0,
        SilentMode_On,
    } silent_mode;
} tone;

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

    tone.state = Tone_Off;
    tone.silent_mode = SilentMode_Off;

    // Enable TIM1 outputs
    //TIM1->BDTR |= TIM_MOE;

    // Enable TIM1
    //TIM1->CTLR1 |= TIM_CEN;

    return TuxSays_Ok;
}

static inline void enable(uint32_t frequency) {
    // calc prescaller and atrlr
    uint32_t presc = 4;
    uint32_t atrlr = FUNCONF_SYSTEM_CORE_CLOCK / frequency / presc;

    while(atrlr > 65000) {
        presc *= 2;
        atrlr = FUNCONF_SYSTEM_CORE_CLOCK / frequency / presc;
    }

    TIM1->CH2CVR = ((atrlr * CONFIG_BEEPER_VOLUME) / CONFIG_BEEPER_VOLUME_BASE) + 1;
    TIM1->PSC = presc;
    TIM1->ATRLR = atrlr;
    TIM1->CNT = 0;

    if(tone.silent_mode == SilentMode_Off) {
        // Enable TIM1 outputs
        TIM1->BDTR |= TIM_MOE;
    }

    // Enable TIM1
    TIM1->CTLR1 |= TIM_CEN;
}

static inline void disable() {
    // Disable IRQ
    TIM1->DMAINTENR &= ~TIM_IT_Update;
    NVIC_DisableIRQ(TIM1_UP_IRQn);

    // Disable TIM1 outputs
    TIM1->BDTR &= ~TIM_MOE;

    // Disable TIM1
    TIM1->CTLR1 &= ~TIM_CEN;

    tone.state = Tone_Off;
}

static inline void play_gen() {
    // reveal first frequency
    uint32_t f = tone.generator.cb((void*)tone.generator.ctx);
    if(f != 0) {
        NVIC_EnableIRQ(TIM1_UP_IRQn);
        TIM1->INTFR = ~TIM_FLAG_Update;
        TIM1->DMAINTENR |= TIM_IT_Update;

        tone.generator.next_freq = tone.generator.cb((void*)tone.generator.ctx);
        enable(f);
    } else {
        disable();
        TuxSays_Task_WakeTask((TuxSays_Task_Handle*)tone.generator.waiting_task);
    }
}

void TIM1_UP_IRQHandler(void) __attribute__((interrupt));
void TIM1_UP_IRQHandler() {
    if(TIM1->INTFR & TIM_FLAG_Update) {
        TIM1->INTFR = ~TIM_FLAG_Update;

        //uint32_t f = tone.generator.cb((void*)tone.generator.ctx);
        if(tone.generator.next_freq != 0) {
            enable(tone.generator.next_freq);
            tone.generator.next_freq = tone.generator.cb((void*)tone.generator.ctx);
        } else {
            disable();
            TuxSays_Task_WakeTask((TuxSays_Task_Handle*)tone.generator.waiting_task);
        }
    }
}

TuxSays_Error TuxSays_Tone_Gen(TuxSays_Tone_Generator generator, void* ctx) {
    if(tone.state != Tone_Off) {
        return TuxSays_Error_ResourceBusy;
    }

    // store state
    tone.state = Tone_PlayGenerator;
    tone.generator.cb = generator;
    tone.generator.ctx = ctx;
    tone.generator.next_freq = (uint32_t)-1;

    TuxSays_PowerMgmt_LockSleep();
    tone.generator.waiting_task = TuxSays_Task_GetCurrentTask();
    play_gen();
    TuxSays_Task_WaitForEvent(TS_TASK_WAIT_FOREVER);
    TuxSays_PowerMgmt_UnlockSleep();

    return TuxSays_Ok;
}

TuxSays_Error TuxSays_Tone(uint32_t frequency, uint32_t duration) {
    if(tone.state != Tone_Off) {
        return TuxSays_Error_ResourceBusy;
    }

    TS_LOG_D("play tone %lu for %lu ms", frequency, duration);
    if(frequency > 0) {
        TuxSays_PowerMgmt_LockSleep();
        tone.state = Tone_Play;
        enable(frequency);
    }

    CHECK_ERR(TuxSays_Task_Delay(duration));

    TuxSays_PowerMgmt_UnlockSleep();

    disable();

    return TuxSays_Ok;
}

TuxSays_Error TuxSays_Tone_SetSilentMode(uint32_t state) {
    tone.silent_mode = state;
    return TuxSays_Ok;
}
