#pragma once

#include "config.h"
#include "utils.h"

#define TS_LOG_DEBUG 'D'
#define TS_LOG_ERROR 'E'
#define TS_LOG_INFO 'I'
#define TS_LOG_WARN 'W'

#if defined(TS_LOG_DEFALUT) && TS_LOG_DEFALUT
#define TS_LOG(_lvl, _msg, ...) TuxSays_Log(_lvl, TAG, _msg, ##__VA_ARGS__)
#define TS_LOG_TAG(_name) static const char __attribute__((unused)) TAG[] = _name
#endif

#ifndef TS_LOG
#define TS_LOG(_lvl, _msg, ...)
#define TS_LOG_TAG(_name)
#endif

#define TS_LOG_I(_msg, ...) TS_LOG(TS_LOG_INFO, _msg, ##__VA_ARGS__)
#define TS_LOG_W(_msg, ...) TS_LOG(TS_LOG_WARN, _msg, ##__VA_ARGS__)
#define TS_LOG_E(_msg, ...) TS_LOG(TS_LOG_ERROR, _msg, ##__VA_ARGS__)
#define TS_LOG_D(_msg, ...) TS_LOG(TS_LOG_DEBUG, _msg, ##__VA_ARGS__)

void TuxSays_Log(const char level, const char* tag, const char* message, ...);