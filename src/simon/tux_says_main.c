#include "ch32v003fun.h"

#include "ch32v003_GPIO_branchless.h"
#include <stdio.h>

#include "ch32v003fun_missing.h"
#include "pin_mapping_config.h"
#include "logger.h"
#include "systick.h"
#include "leds.h"
#include "tone.h"
#include "buttons.h"
#include "pitches.h"
#include "tasks.h"
#include "game.h"

TS_LOG_TAG("MAIN");
/*
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
        //TuxSays_Tone(melody[thisNote], noteDuration);
        // to distinguish the notes, set a minimum time between them.
        // the note's duration + 30% seems to work well:
        int pauseBetweenNotes = (noteDuration * 3) / 10;
        TuxSays_SysTick_DelaySync(pauseBetweenNotes);
        // stop the tone playing:
    }
}
*/
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

//void task_run_main() __attribute__((noinline));
void task_main_run() {
    TS_LOG_I("Started main task");

    INIT_CHECK_ERR(TuxSays_Game_Start(DEFAULT_GAME), "unable to start default game");

    for(;;) {
        LOOP_CHECK_ERR(TuxSays_Game_Loop(), "unable to loop game");

        /*
        uint32_t result;
        TuxSays_Error err = TuxSays_Buttons_Read(&result, TS_TASK_WAIT_FOREVER);

        if(err == TuxSays_Error_Timeout) {
            TS_LOG_I("No button");
        } else if(err == TuxSays_Ok) {
            TS_LOG_I("Button %08X", result);
        } else {
            INIT_CHECK_ERR(err, "unable to read buttons");
        }
        */
        //TuxSays_Task_DumpStackInfo();
    }
}

void silent_beeper_on_start() {
    RCC->APB2PCENR |= GPIO_BEEPER_BUS;
    GPIO_pinMode(
        GPIOv_from_PORT_PIN(GPIO_BEEPER_PORT, GPIO_BEEPER_PIN),
        GPIO_pinMode_O_pushPull,
        GPIO_Speed_2MHz);
    GPIO_digitalWrite(GPIOv_from_PORT_PIN(GPIO_BEEPER_PORT, GPIO_BEEPER_PIN), low);
    RCC->APB2PCENR &= ~GPIO_BEEPER_BUS;
}

int main() {
    SystemInit();
    silent_beeper_on_start();

    TS_LOG_I("========= STARTING =========");

    TuxSays_SysTick_Init();

    TuxSays_SysTick_Delay(2000);

    INIT_CHECK_ERR(TuxSays_Task_Init(), "unable to init tasks");
    TS_LOG_I("Started");

    INIT_CHECK_ERR(TuxSays_Leds_Init(), "unable to init leds");
    INIT_CHECK_ERR(TuxSays_Tone_Init(), "unable to init tone");
    INIT_CHECK_ERR(TuxSays_Buttons_Init(), "unable to init buttons");

    //
    //TuxSays_Game_Start(&game_test);

    //INIT_CHECK_ERR(TuxSays_Task_Start(TuxSays_Taskmain), "unable to start task");

    INIT_CHECK_ERR(TuxSays_Task_StartScheduler(), "unable to start scheduler");

    for(;;) {
        //switch_context(&stack, &stack);

        //TuxSays_AsyncContext_create(ctx);
        //poll_input(); // RX from debug iface not used

        //LOOP_CHECK_ERR(TuxSays_Tone_Loop(&ctx), "unable to loop tone generator");
        //LOOP_CHECK_ERR(TuxSays_Game_Loop(&ctx), "unable to loop game");

        //LOOP_CHECK_ERR(LOOP_AWAIT(TuxSays_Tone_Play(&ctx, NOTE_E4, 200)), "unavle to play tone");

        //LOOP_CHECK_ERR(LOOP_AWAIT(TuxSays_SysTick_DelayAsync(&d1, &ctx, 500)), "unable to wait");
        //LOOP_CHECK_ERR(TuxSays_Leds_Set(TuxSays_Led_1, i & 0x1 ? high : low), "unable to blink led");

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
        TuxSays_SysTick_Delay(500);
    }
}