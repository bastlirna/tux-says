#pragma once

#include "errors.h"
#include <stdint.h>

TuxSays_Error TuxSays_Tone_Init();

TuxSays_Error TuxSays_Tone(uint32_t frequency, uint32_t duration);