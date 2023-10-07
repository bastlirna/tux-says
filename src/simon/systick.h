#pragma once

#include "config.h"
#include "errors.h"
#include <stdint.h>

void TuxSays_SysTick_Init();

void TuxSays_SysTick_Delay(uint32_t ms);

uint32_t TuxSays_SysTick_Freze();

void TuxSays_SysTick_Restore(uint32_t elapsed);

uint32_t TuxSays_SysTick_Milis();