#include "ch32v003fun.h"
#include <stdio.h>

// all LEDs are on GPIO C
#define LED_1 6
#define LED_2 0
#define LED_3 4
#define LED_4 3

#define SYSTICK_SR_CNTIF (1 << 0)
#define SYSTICK_CTLR_STE (1 << 0)
#define SYSTICK_CTLR_STIE (1 << 1)
#define SYSTICK_CTLR_STCLK (1 << 2)
#define SYSTICK_CTLR_STRE (1 << 3)
#define SYSTICK_CTLR_SWIE (1 << 31)

volatile uint32_t millis;

/*
 * Start up the SysTick IRQ
 */
void SysTickInit()
{
    /* disable default SysTick behavior */
    SysTick->CTLR = 0;

    /* enable the SysTick IRQ */
    NVIC_EnableIRQ(SysTicK_IRQn);

    /* Set the tick interval to 1ms for normal op */
    SysTick->CMP = (FUNCONF_SYSTEM_CORE_CLOCK / (1000)) - 1;

    /* Start at zero */
    SysTick->CNT = 0;
    millis = 0;

    /* Enable SysTick counter, IRQ, HCLK/1 */
    SysTick->CTLR = SYSTICK_CTLR_STE | SYSTICK_CTLR_STIE | SYSTICK_CTLR_STRE | SYSTICK_CTLR_STCLK;
}

uint8_t x;

/**
 * SysTick ISR just counts ticks
 */
void SysTick_Handler(void) __attribute__((interrupt));
void SysTick_Handler(void)
{
    SysTick->SR = 0; // clear IRQ
    millis++;
}

void setLedOutput(uint8_t pin)
{
    GPIOC->CFGLR &= ~(0xf << (4 * pin));
    GPIOC->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP) << (4 * pin);
}

void setLedPin(uint8_t pin, uint8_t value)
{
    if (value)
        GPIOC->BSHR = (1 << pin);
    else
        GPIOC->BSHR = (1 << (16 + pin));
}

uint8_t getSwitch(uint8_t pin)
{
    switch (pin)
    {
    case 1:
        return (GPIOD->INDR & (1 << 4)) == 0;
    case 2:
        return (GPIOA->INDR & (1 << 2)) == 0;
    case 3:
        return (GPIOD->INDR & (1 << 6)) == 0;
    case 4:
        return (GPIOA->INDR & (1 << 1)) == 0;
    default:
        return 0;
    }
}

void setLed(uint8_t led, uint8_t value)
{
    switch (led)
    {
    case 1:
        setLedPin(LED_1, value);
        break;
    case 2:
        setLedPin(LED_2, value);
        break;
    case 3:
        setLedPin(LED_3, value);
        break;
    case 4:
        setLedPin(LED_4, value);
        break;
    }
}

void GpioInit()
{
    // Enable GPIOs
    RCC->APB2PCENR |= RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD;

    // LEDs:

    // GPIO C6 (LED 1)
    GPIOC->CFGLR &= ~(0xf << (4 * 6));
    GPIOC->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP) << (4 * 6);

    // GPIO C0 (LED 0)
    GPIOC->CFGLR &= ~(0xf << (4 * 0));
    GPIOC->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP) << (4 * 0);

    // GPIO C4 (LED 3)
    GPIOC->CFGLR &= ~(0xf << (4 * 4));
    GPIOC->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP) << (4 * 4);

    // GPIO C3 (LED 4)
    GPIOC->CFGLR &= ~(0xf << (4 * 3));
    GPIOC->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP) << (4 * 3);

    // GPIO D5 (UTX)
    GPIOD->CFGLR &= ~(0xf << (4 * 5));
    GPIOD->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP) << (4 * 5);

    // Switches:

    // GPIO PD4 (SW 1)
    GPIOD->CFGLR &= ~(0xf << (4 * 4));
    GPIOD->CFGLR |= (GPIO_CNF_IN_PUPD) << (4 * 4);
    GPIOD->BSHR |= 1 << 4;

    // GPIO PA2 (SW 2)
    GPIOA->CFGLR &= ~(0xf << (4 * 2));
    GPIOA->CFGLR |= (GPIO_CNF_IN_PUPD) << (4 * 2);
    GPIOA->BSHR |= 1 << 2;

    // GPIO PD6 (SW 3)
    GPIOD->CFGLR &= ~(0xf << (4 * 6));
    GPIOD->CFGLR |= (GPIO_CNF_IN_PUPD) << (4 * 6);
    GPIOD->BSHR |= 1 << 6;

    // GPIO PA1 (SW 4)
    GPIOA->CFGLR &= ~(0xf << (4 * 1));
    GPIOA->CFGLR |= (GPIO_CNF_IN_PUPD) << (4 * 1);
    GPIOA->BSHR |= 1 << 1;
}

uint32_t counter;

void delay(uint32_t ms)
{
    uint32_t start = millis;
    while (millis - start < ms)
        ;
}

int main()
{
    SystemInit();
    SysTickInit();

    GpioInit();

    for (size_t i = 0; i < 8; i++)
    {
        setLed(i % 4 + 1, 1);
        delay(100);
        setLed(i % 4 + 1, 0);
        delay(20);
    }

    while (1)
    {
        uint8_t sw1 = getSwitch(1);
        uint8_t sw2 = getSwitch(2);
        uint8_t sw3 = getSwitch(3);
        uint8_t sw4 = getSwitch(4);

        printf("SW %d %d %d %d \n", sw1, sw2, sw3, sw4);

        setLed(1, sw1);
        setLed(2, sw2);
        setLed(3, sw3);
        setLed(4, sw4);

        delay(200);
    }
}
