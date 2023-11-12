#include "ch32v003fun.h"
#include <stdio.h>

static inline void setup_pin(GPIO_TypeDef* gpio, uint32_t pin, uint32_t mode) {
    gpio->CFGLR &= ~(0xf << (4 * pin));
    gpio->CFGLR |= mode << (4 * pin);
}

static inline void write_pin(GPIO_TypeDef* gpio, uint32_t pin, uint32_t mode) {
    if(mode == 0) {
        gpio->BSHR |= (1 << pin);
    } else {
        gpio->BSHR |= (1 << (pin + 16));
    }
}

int main() {
    SystemInit();

    Delay_Ms(100);

    RCC->APB2PCENR |= RCC_APB2Periph_GPIOC; // Enable GPIOC clock

    // set-up & disable beeper
    setup_pin(GPIOC, 7, GPIO_Speed_2MHz | GPIO_CNF_OUT_PP); // low speed push/pull mode
    write_pin(GPIOC, 7, 1); // set to log1 = disable beeper

    // set-up led A as output
    setup_pin(GPIOC, 6, GPIO_Speed_2MHz | GPIO_CNF_OUT_PP); // low speed push/pull mode
    write_pin(GPIOC, 6, 1); //set to log1

    for(;;) {
        write_pin(GPIOC, 6, 0);
        Delay_Ms(500);
        write_pin(GPIOC, 6, 1);
        Delay_Ms(500);
    }
}