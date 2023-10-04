// Code mostly from https://github.com/sparkfun/Simon-Says/

#include "memory_game.h"

#include "../game.h"
#include "../buttons.h"
#include "../leds.h"
#include "../systick.h"
#include "../tone.h"
#include "../tasks.h"
#include "../ch32v003fun_missing.h"

#include "game_choices.h"
#include "game_utils.h"

TS_LOG_TAG("G_Memory");

// --- Game definitions ---

//Number of rounds to succesfully remember before you win. 13 is do-able.
#define ROUNDS_TO_WIN 12

//Amount of time to press a button before game times out. 3000ms = 3 sec
#define ENTRY_TIME_LIMIT 3000

// --- Game state variables ---

static struct {
    uint32_t round;
    uint32_t seed;
    uint8_t board[32];
    uint32_t current_move;
} game;

// --- Game static function ---

// Adds a new random button to the game sequence, by sampling the timer
static TuxSays_Error add_to_moves() {
    uint8_t newButton = (preudo_rand_r(&game.seed) - 1) % 4;

    // We have to convert this number, 0 to 3, to CHOICEs
    if(newButton == 0)
        newButton = CHOICE_RED;
    else if(newButton == 1)
        newButton = CHOICE_GREEN;
    else if(newButton == 2)
        newButton = CHOICE_BLUE;
    else if(newButton == 3)
        newButton = CHOICE_YELLOW;

    game.board[game.round++] = newButton; // Add this new button to the game array

    return TuxSays_Ok;
}

// Plays the current contents of the game moves
static TuxSays_Error play_moves() {
    for(uint32_t currentMove = 0; currentMove < game.round; currentMove++) {
        //
        TuxSays_Game_toner(game.board[currentMove], 150);

        // Wait some amount of time between button playback
        // Shorten this to make game harder
        TuxSays_Task_Delay(150); // 150 works well. 75 gets fast.
    }

    return TuxSays_Ok;
}

static TuxSays_Error play_memory(TuxSays_Game_Result* result) {
    //randomSeed(millis()); // Seed the random generator with random amount of millis()
    game.seed = ESIG->UID0 & ESIG->UID1 & ESIG->UID2 & TuxSays_SysTick_Milis();

    game.round = 0; // Reset the game to the beginning

    while(game.round < ROUNDS_TO_WIN) {
        if(game.round > 0) {
            TuxSays_Task_Delay(1000); // Player was correct, delay before playing moves
        }

        add_to_moves(); // Add a button to the current moves, then play them back

        play_moves(); // Play back the current game board

        // Then require the player to repeat the sequence.
        for(uint32_t currentMove = 0; currentMove < game.round; currentMove++) {
            uint32_t choice = 0;
            TuxSays_Game_wait_for_button(&choice, 3000);
            TS_LOG_I("choice = %lu current = %lu", choice, game.board[currentMove]);

            if(choice == 0) {
                *result = TuxSays_Game_PlayerLose;
                return TuxSays_Ok; // If wait timed out, player loses
            }

            if(choice != game.board[currentMove]) {
                *result = TuxSays_Game_PlayerLose;
                return TuxSays_Ok; // If the choice is incorect, player loses
            }
        }
    }

    *result = TuxSays_Game_PlayerWin;
    return TuxSays_Ok; // Player made it through all the rounds to win!
}

TuxSays_Error TuxSays_Game_memory_start() {
    return TuxSays_Ok;
}

TuxSays_Error TuxSays_Game_memory_loop() {
    TuxSays_Game_Result result;

    // Play memory game and handle result
    CHECK_ERR(play_memory(&result));
    if(result == TuxSays_Game_PlayerWin) {
        CHECK_ERR(TuxSays_Game_play_winner()); // Player won, play winner tones
    } else {
        CHECK_ERR(TuxSays_Game_play_loser()); // Player lost, play loser tones
    }

    TS_LOG_I("Exit test due to inactivity");
    TuxSays_Game_Start(&game_attract);

    return TuxSays_Ok;
}

TuxSays_Error TuxSays_Game_memory_stop() {
    return TuxSays_Ok;
}