#include "ch32v003fun.h"

#include "ch32v003_GPIO_branchless.h"
#include <stdio.h>

#include "logger.h"
#include "systick.h"
#include "leds.h"
#include "tone.h"
#include "buttons.h"
#include "pitches.h"
#include "game.h"

TS_LOG_TAG("MAIN");

const uint16_t melody[] = {NOTE_G4, NOTE_A4, 0,       NOTE_C5, 0,       0,       NOTE_G4, 0,
                           0,       0,       NOTE_E4, 0,       NOTE_D4, NOTE_E4, NOTE_G4, 0,
                           NOTE_D4, NOTE_E4, 0,       NOTE_G4, 0,       0,       NOTE_D4, 0,
                           NOTE_E4, 0,       NOTE_G4, 0,       NOTE_A4, 0,       NOTE_C5, 0};

// This essentially sets the tempo, 115 is just about right for a disco groove :)
uint16_t noteDuration = 115;

void play_beegees() {
    // iterate over the notes of the melody:
    for(uint16_t thisNote = 0; thisNote < 32; thisNote++) {
        //changeLED();
        //tone(BUZZER2, melody[thisNote], noteDuration);
        TuxSays_Tone(melody[thisNote], noteDuration);
        // to distinguish the notes, set a minimum time between them.
        // the note's duration + 30% seems to work well:
        int pauseBetweenNotes = (noteDuration * 3) / 10;
        TuxSays_SysTick_Delay(pauseBetweenNotes);
        // stop the tone playing:
    }
}

void halt_and_reboot() {
    // TODO: implement me :)
}

// Different behavior of check error on main
#define INIT_CHECK_ERR(fn, msg, ...)            \
    {                                           \
        TuxSays_Error err = fn;                 \
        if(err != TuxSays_Ok) {                 \
            LOG_ERR_M(err, msg, ##__VA_ARGS__); \
            halt_and_reboot();                  \
        }                                       \
    }

#define LOOP_CHECK_ERR(fn, msg, ...)            \
    {                                           \
        TuxSays_Error err = fn;                 \
        if(err != TuxSays_Ok) {                 \
            LOG_ERR_M(err, msg, ##__VA_ARGS__); \
            TuxSays_SysTick_Delay(500);         \
        }                                       \
    }

int main() {
    SystemInit();
    TuxSays_SysTick_Init();

    TuxSays_SysTick_Delay(100);

    TS_LOG_I("Started");

    INIT_CHECK_ERR(TuxSays_Leds_Init(), "unable to init leds");
    INIT_CHECK_ERR(TuxSays_Tone_Init(), "unable to init tone");
    INIT_CHECK_ERR(TuxSays_Buttons_Init(), "unable to init buttons");

    INIT_CHECK_ERR(TuxSays_Game_Start(DEFAULT_GAME), "unable to start default game");
    //TuxSays_Game_Start(&game_test);

    for(;;) {
        //poll_input(); // RX from debug iface not used

        LOOP_CHECK_ERR(TuxSays_Game_Loop(), "unable to loop game");

        //TuxSays_Game_Init();
        /*
        //TuxSays_Buttons_Loop();
        if(TuxSays_Buttons_Get(TuxSays_Button_1) == high) {
            TuxSays_Leds_Blink(TuxSays_Led_1, 100);
            TuxSays_Leds_Blink(TuxSays_Led_2, 100);
            //TuxSays_Tone(NOTE_E4, 100);
            play_beegees();
        }
        Delay(500);
        TuxSays_Leds_Blink(TuxSays_Led_2, 100);
        */
    }
}