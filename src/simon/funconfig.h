#ifndef _FUNCONFIG_H
#define _FUNCONFIG_H

#include "config.h"

#ifdef TS_LOG_DEFALUT
#define FUNCONF_USE_DEBUGPRINTF 1
#define FUNCONF_DEBUGPRINTF_TIMEOUT (1 << 31) // Wait for a very very long time.
#endif

#define FUNCONF_USE_PLL 0

#define CH32V003 1

#endif
