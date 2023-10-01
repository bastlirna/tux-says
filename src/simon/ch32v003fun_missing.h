#pragma once

#include "config.h"
#include <stdarg.h>

#define SYSTICK_SR_CNTIF (1 << 0)
#define SYSTICK_CTLR_STE (1 << 0)
#define SYSTICK_CTLR_STIE (1 << 1)
#define SYSTICK_CTLR_STCLK (1 << 2)
#define SYSTICK_CTLR_STRE (1 << 3)
#define SYSTICK_CTLR_SWIE (1 << 31)

#ifndef GPIOPortByBase
#define GPIOPortByBase(i) ((GPIO_TypeDef*)(GPIOA_BASE + 0x0400 * (i)))
#endif

#ifdef EMULATE_VPRINTF
int missing_vprintf(const char* message, va_list args);
#endif