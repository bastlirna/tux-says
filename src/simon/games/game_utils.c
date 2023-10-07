#include "game_utils.h"

#include "../game.h"
#include "../buttons.h"
#include "../leds.h"
#include "../systick.h"
#include "../tone.h"
#include "../tasks.h"
#include "game_choices.h"

TS_LOG_TAG("G_Utils");

// Light an LED and play tone
TuxSays_Error TuxSays_Game_toner(uint32_t choice, uint32_t buzz_length_ms) {
    TS_LOG_D("toner %08X", choice);
    TuxSays_Leds_Write(choice); //Turn on a given LED

    //Play the sound associated with the given LED
#define LED_COLOR(_id, _name, _tone)                    \
    if(choice & CHOICE_##_name) {                       \
        choice = 0;                                     \
        TS_LOG_D("choice " #_name);                     \
        CHECK_ERR(TuxSays_Tone(_tone, buzz_length_ms)); \
    }
#include "led_color_config.h"
#undef LED_COLOR

    TuxSays_Leds_Write(CHOICE_NONE); // Turn off all LEDs

    return TuxSays_Ok;
}

TuxSays_Error TuxSays_Game_wait_for_button(uint32_t* result, uint32_t timeout) {
    *result = 0;
    uint32_t active_buttons = 0;
    TuxSays_Error err = TuxSays_Buttons_Read(&active_buttons, timeout);
    if(err == TuxSays_Error_Timeout) {
        return TuxSays_Error_Timeout;
    }
    CHECK_ERR(err);

    *result = active_buttons;

    TS_LOG_D("active_buttons %08X", active_buttons);

    if(active_buttons != CHOICE_NONE) {
        TuxSays_Game_toner(active_buttons, 150);

        do {
            // wait for button release
            TuxSays_Buttons_Read(&active_buttons, TS_TASK_NO_BLOCKING);
        } while(active_buttons != 0);
    }

    return TuxSays_Ok;
}

typedef struct {
    uint16_t freq;
} win_melody_generator_ctx;

static uint32_t win_melody_generator(void* ctx) {
    win_melody_generator_ctx* c = (win_melody_generator_ctx*)ctx;

    c->freq += 5;
    if(c->freq >= 6000) {
        return 0;
    }

    return c->freq;
}

// Play the winner sound
// This is just a unique (annoying) sound we came up with, there is no magic to it
static TuxSays_Error winner_sound() {
    win_melody_generator_ctx ctx = {
        .freq = 2000,
    };

    // Toggle the buzzer at various speeds
    // 2k to 7kHz
    CHECK_ERR(TuxSays_Tone_Gen(win_melody_generator, &ctx));

    return TuxSays_Ok;
}

static const uint8_t win_sequence[] = {
    CHOICE_GREEN | CHOICE_BLUE,
    CHOICE_RED | CHOICE_YELLOW,
    CHOICE_GREEN | CHOICE_BLUE,
    CHOICE_RED | CHOICE_YELLOW,
};

// Play the winner sound and lights
TuxSays_Error TuxSays_Game_play_winner() {
    for(uint32_t i = 0; i < sizeof(win_sequence); i++) {
        CHECK_ERR(TuxSays_Leds_Write(win_sequence[i]));
        CHECK_ERR(winner_sound());
    }
    return TuxSays_Ok;
}

static const uint8_t lose_sequence[] = {
    CHOICE_RED | CHOICE_GREEN,
    CHOICE_BLUE | CHOICE_YELLOW,
    CHOICE_RED | CHOICE_GREEN,
    CHOICE_BLUE | CHOICE_YELLOW,
};

TuxSays_Error TuxSays_Game_play_loser() {
    for(uint32_t i = 0; i < sizeof(lose_sequence); i++) {
        CHECK_ERR(TuxSays_Leds_Write(lose_sequence[i]));
        CHECK_ERR(TuxSays_Tone(1500, 255));
    }
    return TuxSays_Ok;
}
