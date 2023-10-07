#include <stdint.h>

#include "ch32v003fun.h"
#include "ch32v003fun_missing.h"
#include "systick.h"
#include "tasks.h"
#include "power_management.h"

struct {
    enum {
        BlockSleepModeFromStart = 0,
        AllowSleepMode,
    } state;
    uint32_t started_at;
} task;

static void init_autowake() {
    RCC->APB1PCENR |= RCC_APB1Periph_PWR;

    // enable low speed oscillator (LSI)
    RCC->RSTSCKR |= RCC_LSION;
    while((RCC->RSTSCKR & RCC_LSIRDY) == 0) {
    }

    // enable AutoWakeUp event
    EXTI->EVENR |= EXTI_Line9;
    EXTI->FTENR |= EXTI_Line9;

    // configure AWU prescaler
    PWR->AWUPSC |= PWR_AWU_Prescaler_32;

    // configure AWU window comparison value
    //PWR->AWUWR &= ~0x3f;
    //PWR->AWUWR |= 63;

    // enable AWU
    //PWR->AWUCSR |= (1 << 1);

    PWR->CTLR &= ~PWR_CTLR_PDDS;
}

static const uint32_t presc_table[] =
    {1, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 10240, 61440};

static inline uint32_t prepare_prescaler(uint32_t* delay) {
    uint32_t presc = PWR_AWU_Prescaler_128;
    uint32_t awuwr = *delay;

    while(awuwr > 0x1f && presc < ((sizeof(presc_table) / sizeof(uint32_t)) - 1)) {
        presc++;
        awuwr = *delay / ((presc_table[presc] * 1000) / AWUCSR_BASE_FREQUENCY);
    }

    if(awuwr > 0x1f) {
        awuwr = 0x1f;
    }

    *delay = (awuwr * presc_table[presc]) / (AWUCSR_BASE_FREQUENCY / 1000);

    PWR->AWUWR = awuwr;
    PWR->AWUPSC = presc;

    return awuwr;
}

//void task_idle_run() __attribute__((interrupt));
void task_idle_run() {
    task.state = BlockSleepModeFromStart;
    task.started_at = TuxSays_SysTick_Milis();

    init_autowake();

    for(;;) {
        if(task.state == BlockSleepModeFromStart) {
            if((TuxSays_SysTick_Milis() - task.started_at) > BLOCK_SLEEP_MODE_TIMEOUT) {
                task.state = AllowSleepMode;
            }
        } else if(task.state == AllowSleepMode && !TuxSays_PowerMgmt_IsSleepLocked()) {
#if defined(CONFIG_SLEEP_MODE) && CONFIG_SLEEP_MODE

            uint32_t delay;
            TuxSays_Task_CalculateSleepTime(&delay);

            if(prepare_prescaler(&delay) > 0) {
                PWR->AWUCSR |= PWR_AWUCSR_AWUEN;
                //PWR->CTLR |= PWR_CTLR_PDDS;

                TuxSays_SysTick_Freze();
                PFIC->SCTLR |= PFIC_SCTLR_SLEEPDEEP;
                __WFE();
                TuxSays_SysTick_Restore(delay);

                // restore clock domain after wakeup
                SystemInit();

                PWR->AWUCSR &= ~PWR_AWUCSR_AWUEN;
                //PWR->CTLR &= ~PWR_CTLR_PDDS;
                PFIC->SCTLR &= ~PFIC_SCTLR_SLEEPDEEP;
            }
#endif
        }

        TuxSays_Task_Yield();
    }
}
