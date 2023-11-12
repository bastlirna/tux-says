#include "ch32v003fun.h"
#include "ch32v003fun_missing.h"
#include <stdio.h>

static inline void set_exticr(uint16_t port, uint16_t pin) {
    AFIO->EXTICR |= (port << (pin * 2));
}

static inline void setup_pin(GPIO_TypeDef* gpio, uint32_t pin, uint32_t mode) {
    gpio->CFGLR &= ~(0xf << (4 * pin));
    gpio->CFGLR |= mode << (4 * pin);
}

static inline void write_pin(GPIO_TypeDef* gpio, uint32_t pin, uint32_t mode) {
    if(mode != 0) {
        gpio->BSHR |= (1 << pin);
    } else {
        gpio->BSHR |= (1 << (pin + 16));
    }
}

static void init_gpio() {
    RCC->APB2PCENR |= RCC_APB2Periph_GPIOC; // Enable GPIOC clock
    RCC->APB2PCENR |= RCC_APB2Periph_GPIOD; // Enable GPIOD clock
    RCC->APB2PCENR |= RCC_APB2Periph_GPIOA; // Enable GPIOA clock

    // set-up & disable beeper
    setup_pin(GPIOC, 7, GPIO_Speed_2MHz | GPIO_CNF_OUT_PP); // low speed push/pull mode
    write_pin(GPIOC, 7, 0); // set to log0

    // Init leds

    // set-up led A as output
    setup_pin(GPIOC, 6, GPIO_Speed_2MHz | GPIO_CNF_OUT_PP); // low speed push/pull mode
    write_pin(GPIOC, 6, 0); //set to log0

    // set-up led B as output
    setup_pin(GPIOC, 0, GPIO_Speed_2MHz | GPIO_CNF_OUT_PP); // low speed push/pull mode
    write_pin(GPIOC, 0, 0); //set to log0

    // set-up led C as output
    setup_pin(GPIOC, 4, GPIO_Speed_2MHz | GPIO_CNF_OUT_PP); // low speed push/pull mode
    write_pin(GPIOC, 4, 0); //set to log0

    // set-up led C as output
    setup_pin(GPIOC, 3, GPIO_Speed_2MHz | GPIO_CNF_OUT_PP); // low speed push/pull mode
    write_pin(GPIOC, 3, 0); //set to log0

    // Init buttons

    // set-up button A as input
    setup_pin(GPIOD, 4, GPIO_CNF_IN_PUPD); // pull-up/down mode
    write_pin(GPIOD, 4, 1); // set as pull-up

    // set-up button B as input
    setup_pin(GPIOA, 2, GPIO_CNF_IN_PUPD); // pull-up/down mode
    write_pin(GPIOA, 2, 1); // set as pull-up

    // set-up button C as input
    setup_pin(GPIOD, 6, GPIO_CNF_IN_PUPD); // pull-up/down mode
    write_pin(GPIOD, 6, 1); // set as pull-up

    // set-up button D as input
    setup_pin(GPIOA, 1, GPIO_CNF_IN_PUPD); // pull-up/down mode
    write_pin(GPIOA, 1, 1); // set as pull-up
}

void init_auto_wakeup() {
    // enable low speed oscillator (LSI)
    RCC->RSTSCKR |= RCC_LSION;

    // Wait for LSI ready flag
    while((RCC->RSTSCKR & RCC_LSIRDY) == 0) {
    }

    // Enable event on EXTI line 9 (Auto-Wakeup line)
    EXTI->EVENR |= EXTI_Line9;

    // Enable falling edge trigger on EXTI line 9
    EXTI->FTENR |= EXTI_Line9;

    // Configure Auto-Wake timer prescaller
    //PWR->AWUPSC |= PWR_AWU_Prescaler_4096;
    PWR->AWUPSC |= PWR_AWU_Prescaler_10240;

    // Configure window comparsion register
    PWR->AWUWR = 31;
}

void init_event_wakeup() {
    RCC->APB2PCENR |= RCC_APB2Periph_AFIO; // Enable AFIO clock

    // Enable GPIO external interrupt
    set_exticr(AFIO_EXTICR1_EXTI0_PD, 4); // Button A
    set_exticr(AFIO_EXTICR1_EXTI0_PA, 2); // Button B
    set_exticr(AFIO_EXTICR1_EXTI0_PD, 6); // Button C
    set_exticr(AFIO_EXTICR1_EXTI0_PA, 1); // Button D

    // Configure EXTI as events
    EXTI->EVENR |= (1 << 4); // EXTI4
    EXTI->EVENR |= (1 << 2); // EXTI2
    EXTI->EVENR |= (1 << 6); // EXTI6
    EXTI->EVENR |= (1 << 1); // EXTI1

    // Configure falling edge for EXTI line
    EXTI->FTENR |= (1 << 4); // EXTI4
    EXTI->FTENR |= (1 << 2); // EXTI2
    EXTI->FTENR |= (1 << 6); // EXTI6
    EXTI->FTENR |= (1 << 1); // EXTI1
}

static inline void enable_auto_wakeup() {
    // Enable Auto-Wake timer
    PWR->AWUCSR |= PWR_AWUCSR_AWUEN;
}

void init_sleep() {
    // Enable PWR clock
    RCC->APB1PCENR |= RCC_APB1Periph_PWR;

    // Auto-wakeup support
    init_auto_wakeup();

    // Event wakeup
    init_event_wakeup();

    // Standby/ Sleep mode selection bit in power-down deep sleep scenario.
    // 1: Enter Standby mode.
    // 0: Enter Sleep mode.
    //PWR->CTLR &= ~PWR_CTLR_PDDS; // Enter Sleep mode.
    PWR->CTLR |= PWR_CTLR_PDDS; // Enter Standby mode.
}

void enter_sleep() {
    //Low-power mode of the control system  - should be same as PWR_CTLR_PDDS
    // 1: deep sleep
    // 0: sleep
    //PFIC->SCTLR &= ~PFIC_SCTLR_SLEEPDEEP;
    PFIC->SCTLR |= PFIC_SCTLR_SLEEPDEEP;

    // Enable Auto-Wakeup timer
    enable_auto_wakeup();

    __WFE(); // Wait for event
    //__WFI(); // Wait for interrupt

    // restore clock domain after wakeup
    SystemInit();
}

int main() {
    SystemInit();

    // Init LEDs and buttons
    init_gpio();

    // Wait before start
    // This is neccessary because in sleepmode, debug interface doesn't work
    Delay_Ms(2000);

    // Init low power mode
    init_sleep();

    for(;;) {
        // Goto sleep mode
        enter_sleep();

        write_pin(GPIOC, 6, 1);
        Delay_Ms(200);
        write_pin(GPIOC, 6, 0);
        Delay_Ms(500);
    }
}