#include <stdio.h>
#include <stdint.h>
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

uint32_t preudo_rand_r(uint32_t* seed) {
    uint32_t next = *seed;
    uint32_t result;

    next *= 1103515245;
    next += 12345;
    result = (uint32_t)(next / 65536) % 2048;

    next *= 1103515245;
    next += 12345;
    result <<= 10;
    result ^= (uint32_t)(next / 65536) % 1024;

    next *= 1103515245;
    next += 12345;
    result <<= 10;
    result ^= (uint32_t)(next / 65536) % 1024;

    *seed = next;

    return result;
}