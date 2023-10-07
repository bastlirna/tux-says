#include "power_management.h"

static volatile uint32_t sleep_locked = 0;

void TuxSays_PowerMgmt_LockSleep() {
    sleep_locked = 1;
}

void TuxSays_PowerMgmt_UnlockSleep() {
    sleep_locked = 0;
}

uint32_t TuxSays_PowerMgmt_IsSleepLocked() {
    return sleep_locked;
}
