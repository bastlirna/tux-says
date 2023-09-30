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

// definitio of frequnecies for tones
#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978



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

void Tim1PwmInit()
{
    // enable clock for GPIO C and TIM1
    RCC->APB2PCENR |= RCC_APB2Periph_GPIOC | RCC_APB2Periph_TIM1;

    // emable AFIO clock (alternate periferal function module)
    RCC->APB2PCENR |= RCC_APB2Periph_AFIO;

    // Remap CH2 to PC7
    AFIO->PCFR1 |= AFIO_PCFR1_TIM1_REMAP_PARTIALREMAP;

    // GPIO C7 (buzzer)
    GPIOC->CFGLR &= ~(0xf << (4 * 7));
    GPIOC->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP_AF) << (4 * 7);

    // Reset TIM1 to init all regs
	RCC->APB2PRSTR |= RCC_APB2Periph_TIM1;
	RCC->APB2PRSTR &= ~RCC_APB2Periph_TIM1;
	
	// CTLR1: default is up, events generated, edge align
	// SMCFGR: default clk input is CK_INT
	
	// Prescaler 
	TIM1->PSC = 960;
	
	// Auto Reload - sets period
	TIM1->ATRLR = 10;
	
	// Reload immediately
	TIM1->SWEVGR |= TIM_UG;
	
	// Enable CH2 output, positive pol
	TIM1->CCER |= TIM_CC2E | TIM_CC2P;
	
	// CH2 Mode is output, PWM1 (CC1S = 00, OC1M = 110)
	TIM1->CHCTLR1 |= TIM_OC2M_2 | TIM_OC2M_1;
	
	
	// Set the Capture Compare Register value to 50% initially
	TIM1->CH2CVR = 5;
	
	
	// Enable TIM1 outputs
	TIM1->BDTR |= TIM_MOE;
	
	// Enable TIM1
	TIM1->CTLR1 |= TIM_CEN;
}

void setPwnFrequency(uint32_t freq)
{
    // Disable TIM1 outputs
	TIM1->BDTR &= ~TIM_MOE;
	
	// Disable TIM1
	TIM1->CTLR1 &= ~TIM_CEN;

    if (freq == 0)
    {
        // printf("Disabling PWM\n");
    }
    else
    {
        uint16_t x = (48000000 / 960) / freq;
        //printf("Setting PWM frequency to %ud\n", freq);
        TIM1->ATRLR = x;
        TIM1->CH2CVR = x/2;

        // Enable TIM1 outputs
	    TIM1->BDTR |= TIM_MOE;
	
	    // Enable TIM1
	    TIM1->CTLR1 |= TIM_CEN;
    }
}



int main()
{
    SystemInit();
    SysTickInit();

    GpioInit();
    Tim1PwmInit();

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

        //printf("SW %d %d %d %d \n", sw1, sw2, sw3, sw4);

        setLed(1, sw1);
        setLed(2, sw2);
        setLed(3, sw3);
        setLed(4, sw4);

        if(sw1)
        {
            setPwnFrequency(NOTE_E3);
        }
        if(sw2)
        {
            setPwnFrequency(NOTE_CS3);
        }
        if(sw3)
        {
            setPwnFrequency(NOTE_A3);
        }
        if(sw4)
        {
            setPwnFrequency(NOTE_E2);
        }

        delay(100);

        if(!sw1 && !sw2 && !sw3 && !sw4)
              setPwnFrequency(0);
    }
}
