#include "logger.h"
#include <stdio.h>
#include <stdarg.h>
#include "systick.h"
#include "ch32v003fun_missing.h"

void TuxSays_Log(const char level, const char* tag, const char* message, ...) {
    va_list args;
    va_start(args, message);

    uint32_t ms = TuxSays_SysTick_Milis();

    printf("%lu [%c][%s] ", ms, level, tag);
    missing_vprintf(message, args);
    printf("\r\n");

    va_end(args);

    //47159921 [D][DolphinState]
}