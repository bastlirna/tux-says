#pragma once

#include <stdint.h>
#include "logger.h"

typedef enum {
    TuxSays_Ok = 0,

    TuxSays_Error_NoAnyGameRunnig = -2000,

    TuxSays_Error_NotImplemented = -4000,
    TuxSays_Error_InvalidArgument = -4001,
    TuxSays_Error_NoFreeMemory = -4002,
    TuxSays_Error_Timeout = -4003,
} TuxSays_Error;

char* TuxSays_ErrorToString(TuxSays_Error error);

#define LOG_ERR(err)                        \
    ({                                      \
        TS_LOG_E(                           \
            "error '%s' (0x%08X) on %s:%d", \
            TuxSays_ErrorToString(err),     \
            err,                            \
            __FUNCTION__,                   \
            __LINE__);                      \
        err;                                \
    })

#define LOG_ERR_M(err, msg, ...)                  \
    ({                                            \
        TS_LOG_E(                                 \
            "error '%s' (0x%08X) on %s:%d: " msg, \
            TuxSays_ErrorToString(err),           \
            err,                                  \
            __FUNCTION__,                         \
            __LINE__,                             \
            ##__VA_ARGS__);                       \
        err;                                      \
    })

#define CHECK_ERR(fn)            \
    {                            \
        TuxSays_Error err = fn;  \
        if(err != TuxSays_Ok) {  \
            return LOG_ERR(err); \
        }                        \
    }

#define CHECK_ERR_M(fn, msg, ...)                      \
    {                                                  \
        TuxSays_Error err = fn;                        \
        if(err != TuxSays_Ok) {                        \
            return LOG_ERR_M(err, msg, ##__VA_ARGS__); \
        }                                              \
    }
