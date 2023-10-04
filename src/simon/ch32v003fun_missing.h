#pragma once

#include "config.h"
#include <stdarg.h>

#define SYSTICK_SR_CNTIF (1 << 0)
#define SYSTICK_CTLR_STE (1 << 0)
#define SYSTICK_CTLR_STIE (1 << 1)
#define SYSTICK_CTLR_STCLK (1 << 2)
#define SYSTICK_CTLR_STRE (1 << 3)
#define SYSTICK_CTLR_SWIE (1 << 31)

#define PFIC_SCTLR_SYSRESET (1 << 31)
#define PFIC_SCTLR_SETEVENT (1 << 5)
#define PFIC_SCTLR_SEVONPEND (1 << 4)
#define PFIC_SCTLR_WFITOWFE (1 << 3)
#define PFIC_SCTLR_SLEEPDEEP (1 << 2)
#define PFIC_SCTLR_SLEEPONEXIT (1 << 1)

#define AWUCSR_BASE_FREQUENCY 128000

#define PWR_AWUCSR_AWUEN (1 << 1)

#ifndef GPIOPortByBase
#define GPIOPortByBase(i) ((GPIO_TypeDef*)(GPIOA_BASE + 0x0400 * (i)))
#endif

#ifdef EMULATE_VPRINTF
int missing_vprintf(const char* message, va_list args);
#endif

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

uint32_t preudo_rand_r(uint32_t* seed);