#include <stdio.h>

#include "errors.h"

char* TuxSays_ErrorToString(TuxSays_Error error) {
    switch(error) {
    case TuxSays_Ok:
        return "no-error";
    case TuxSays_Error_NoAnyGameRunnig:
        return "no-game-running";
    case TuxSays_Error_NoFreeMemory:
        return "no-free-memory";
    case TuxSays_Error_NotImplemented:
        return "not-implemented";
    case TuxSays_Error_InvalidArgument:
        return "invalid-argument";
    case TuxSays_Error_Timeout:
        return "timeout";
        //default:
        //    break;
    }

    return "<unk>";
}
