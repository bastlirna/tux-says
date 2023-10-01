// Code mostly from https://github.com/sparkfun/Simon-Says/

#include "battle_game.h"

#include "../game.h"
#include "../buttons.h"
#include "../leds.h"
#include "../systick.h"
#include "../tone.h"

#include "game_choices.h"
#include "game_utils.h"

TS_LOG_TAG("G_BTL");

// --- Game definitions ---

//Number of rounds to succesfully remember before you win. 13 is do-able.
#define ROUNDS_TO_WIN 13

//Amount of time to press a button before game times out. 3000ms = 3 sec
#define ENTRY_TIME_LIMIT 3000

#define MODE_MEMORY 0
#define MODE_BATTLE 1
#define MODE_BEEGEES 2

// --- Game state variables ---

//By default, let's play the memory game
//static uint8_t gameMode = MODE_MEMORY;

//Contains the combination of buttons as we advance
static uint8_t gameBoard[32];

//Counts the number of succesful rounds the player has made it through
static uint8_t gameRound = 0;

// --- Game static function ---

static TuxSays_Error wait_for_button(TuxSays_Button* result);

// --- Game utils ---

// Play the special 2 player battle mode
// A player begins by pressing a button then handing it to the other player
// That player repeats the button and adds one, then passes back.
// This function returns when someone loses
static TuxSays_Error play_battle(TuxSays_Game_Result* result) {
    gameRound = 0; // Reset the game frame back to one frame

    while(1) // Loop until someone fails
    {
        TuxSays_Button newButton;
        CHECK_ERR(wait_for_button(&newButton));
        gameBoard[gameRound++] = newButton; // Add this new button to the game array

        // Then require the player to repeat the sequence.
        for(uint8_t currentMove = 0; currentMove < gameRound; currentMove++) {
            TuxSays_Button choice;
            CHECK_ERR(wait_for_button(&choice));

            if(choice == 0) {
                *result = TuxSays_Game_PlayerLose;
                return TuxSays_Ok; // If wait timed out, player loses.
            }

            if(choice != gameBoard[currentMove]) {
                *result = TuxSays_Game_PlayerLose;
                return TuxSays_Ok; // If the choice is incorect, player loses.
            }
        }

        // Give the user an extra 100ms to hand the game to the other player
        TuxSays_SysTick_Delay(100);
    }

    *result = TuxSays_Game_PlayerWin;
    return TuxSays_Ok; // We should never get here
}

// Play the loser sound/lights
void play_loser() {
    TuxSays_Game_setLEDs(CHOICE_RED | CHOICE_GREEN);
    TuxSays_Tone(255, 1500);

    TuxSays_Game_setLEDs(CHOICE_BLUE | CHOICE_YELLOW);
    TuxSays_Tone(255, 1500);

    TuxSays_Game_setLEDs(CHOICE_RED | CHOICE_GREEN);
    TuxSays_Tone(255, 1500);

    TuxSays_Game_setLEDs(CHOICE_BLUE | CHOICE_YELLOW);
    TuxSays_Tone(255, 1500);
}
/*
// Play the winner sound
// This is just a unique (annoying) sound we came up with, there is no magic to it
void winner_sound() {
    // Toggle the buzzer at various speeds
    for(uint32_t freq = 1000; freq > 600; freq -= 10) {
        TuxSays_Tone(freq, 1);
    }
}


// Play the winner sound and lights
void play_winner() {
    TuxSays_Game_setLEDs(CHOICE_GREEN | CHOICE_BLUE);
    winner_sound();
    TuxSays_Game_setLEDs(CHOICE_RED | CHOICE_YELLOW);
    winner_sound();
    TuxSays_Game_setLEDs(CHOICE_GREEN | CHOICE_BLUE);
    winner_sound();
    TuxSays_Game_setLEDs(CHOICE_RED | CHOICE_YELLOW);
    winner_sound();
}
*/
// Wait for a button to be pressed.
// Returns one of LED colors (LED_RED, etc.) if successful, 0 if timed out
static TuxSays_Error wait_for_button(TuxSays_Button* result) {
    uint32_t startTime = TuxSays_SysTick_Milis(); // Remember the time we started the this loop

    while((TuxSays_SysTick_Milis() - startTime) <
          ENTRY_TIME_LIMIT) // Loop until too much time has passed
    {
        TuxSays_Button button = TuxSays_Game_checkButton();

        if(button != TuxSays_Button_None) {
            TuxSays_Game_toner(button, 150); // Play the button the user just pressed

            while(TuxSays_Game_checkButton() != TuxSays_Button_None)
                ; // Now let's wait for user to release button

            TuxSays_SysTick_Delay(10); // This helps with debouncing and accidental double taps

            return button;
        }
    }

    return TuxSays_Error_Timeout; // If we get here, we've timed out!
}

TuxSays_Error TuxSays_Game_battle_start() {
    while(0) {
        TuxSays_Game_Result result;
        play_battle(&result);
        play_loser();
    }

    //winner_sound();
    return TuxSays_Ok;
}

TuxSays_Error TuxSays_Game_battle_loop() {
    return TuxSays_Ok;
}

TuxSays_Error TuxSays_Game_battle_stop() {
    return TuxSays_Ok;
}