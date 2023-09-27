#include "ch32v003fun.h"
#include <stdio.h>

// all LEDs are on GPIO C
#define LED_1 6
#define LED_2 0
#define LED_3 4
#define LED_4 3

void setLedOutput(uint8_t pin)
{
    GPIOC->CFGLR &= ~(0xf << (4 * pin));
    GPIOC->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP) << (4 * pin);
}

void setLed(uint8_t pin, uint8_t value)
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

int main()
{
    SystemInit();

    GpioInit();

    for (size_t i = 0; i < 2; i++)
    {
        setLed(LED_1, 1);
        Delay_Ms(200);
        setLed(LED_1, 0);
        Delay_Ms(200);
    }

    while (1)
    {
        uint8_t sw1 = getSwitch(1);
        uint8_t sw2 = getSwitch(2);
        uint8_t sw3 = getSwitch(3);
        uint8_t sw4 = getSwitch(4);

        printf("SW %d %d %d %d \n", sw1, sw2, sw3, sw4);

        setLed(LED_1, sw1);
        setLed(LED_2, sw2);
        setLed(LED_3, sw3);
        setLed(LED_4, sw4);

        Delay_Ms(250);
    }

    /*
    while(1)
    {
        setLed(LED_1, 1);
        Delay_Ms( 250 );
        setLed(LED_1, 0);
        setLed(LED_2, 1);
        Delay_Ms( 250 );
        setLed(LED_2, 0);
        setLed(LED_3, 1);
        Delay_Ms( 250 );
        setLed(LED_3, 0);
        setLed(LED_4, 1);
        Delay_Ms( 250 );
        setLed(LED_4, 0);
        Delay_Ms( 500 );
    }
    */
}
