#include <stdio.h>
#include <stdarg.h>
#include "config.h"
#include "ch32v003fun_missing.h"

#ifdef EMULATE_VPRINTF
int _write(int fd, const char* buf, int size);
int mini_vpprintf(int (*puts)(char* s, int len, void* buf), void* buf, const char* fmt, va_list va);

static int __puts_uart(char* s, int len, void* buf) {
    _write(0, s, len);
    return len;
}

int missing_vprintf(const char* format, va_list args) {
    return mini_vpprintf(__puts_uart, 0, format, args);
}
#endif
