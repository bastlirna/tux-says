#pragma once

#include "errors.h"
#include <stdint.h>

typedef uint32_t (*TuxSays_Tone_Generator)(void* ctx);

TuxSays_Error TuxSays_Tone_Init();

TuxSays_Error TuxSays_Tone(uint32_t frequency, uint32_t duration);

TuxSays_Error TuxSays_Tone_Gen(TuxSays_Tone_Generator generator, void* ctx);
