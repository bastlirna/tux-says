#pragma once

#include <stdint.h>
#include "errors.h"

void TuxSays_PowerMgmt_LockSleep();

void TuxSays_PowerMgmt_UnlockSleep();

uint32_t TuxSays_PowerMgmt_IsSleepLocked();