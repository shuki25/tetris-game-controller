/**
 ******************************************************************************
 * @file           : game_loop.c
 * @author         : Patrick Lebeau Jr., Dr. Joshua Butler
 * @date           : Dec 9, 2024
 * @brief          : Main game loop for Classic Tetris on LED Grid
 ******************************************************************************
 * @attention
 *
 * 2025 Imagine RIT Project: Classic Tetris on LED Grid
 *
 * Copyright (c) 2024-25 Rochester Institute of Technology.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include <stdint.h>
#include <string.h>

#include "main.h"
#include "snes_controller.h"
#include "tetrimino.h"
#include "tetrimino_shape.h"
#include "game_loop.h"
#include "itm_debug.h"
#include "util.h"
#include "cmsis_os.h"

uint32_t game_loop_counter = 0;
snes_controller_t snes_controller;
game_t game;
tetrimino_t tetrimino;
tetrimino_t tetrimino_pending;

/**
 * @brief  Initialize game state
 * @param  None
 * @retval None
 */
game_status_t game_init(void) {
    // TODO: Initialize game state (structs, bitboards, etc.)
    memset(&game, 0, sizeof(game_t));
    game.state = GAME_STATE_SPLASH;
    game.score = 0;
    game.level = 0;
    game.lines = 0;
    game.game_speed = 1000;

    return GAME_OK;
}

/**
 * @brief  Main game loop for Classic Tetris on LED Grid
 * @param  None
 * @retval None
 */
void game_loop(void) {
    snes_controller_status_t controller_status;
//    char output_buffer[80];
    tetrimino_t tetrimino;
    tetrimino_status_t tetrimino_status;

    // TODO: Load settings from EEPROM

    // TODO: Load high scores from EEPROM

    // TODO: Initialize game variables

    // TODO: Initialize game components
    controller_status = snes_controller_init(&snes_controller, SNES_LATCH_GPIO_Port, SNES_LATCH_Pin,
    SNES_CLOCK_GPIO_Port, SNES_CLOCK_Pin, SNES_DATA0_GPIO_Port, SNES_DATA0_Pin, 60);
    if (controller_status != SNES_CONTROLLER_OK) {
#if DEBUG_OUTPUT
        printf("SNES controller initialization failed\n");
#endif
    }

    tetrimino_status = tetrimino_init(&tetrimino);
#if DEBUG_OUTPUT
    if (tetrimino_status == TETRIMINO_OK) {
        tetrimino_debug_print(&tetrimino);
    }
#endif

    // TODO: Start the main game loop

    // If you want to test a feature, uncomment the following line
    // game.state = GAME_STATE_TEST_FEATURE;
    game.state = GAME_STATE_GAME_IN_PROGRESS;

    for (;;) {
        // TODO: Respond to scoreboard requests

        // TODO: Poll SNES controller before any other processing in the state machine
        controller_status = snes_controller_read(&snes_controller);
        if (controller_status == SNES_CONTROLLER_STATE_CHANGE) {

        }

        switch (game.state) {

        /* ---------------------- SPLASH SCREEN ---------------------- */
        case GAME_STATE_SPLASH:
            // TODO: Display splash screen

            // TODO: Wait for user input to start game
            game.state = GAME_STATE_SPLASH_WAIT;
            break;

            /* ------------------------- SPLASH WAIT ------------------------ */
        case GAME_STATE_SPLASH_WAIT:
            // TODO: Wait for user input to start game
            break;

            /* ------------------------- MAIN MENU -------------------------- */
        case GAME_STATE_MENU:
            // TODO: Display main menu
            break;

            /* ------------------------ PLAYING MENU ------------------------ */
        case GAME_STATE_PLAY_MENU:
            // TODO: Display playing menu
            break;

            /* -------------------- PREPARE GAME STATE ---------------------- */
        case GAME_STATE_PREPARE_GAME:
            // TODO: Initialize game variables
            break;

            /* ---------------------- GAME IN PROGRESS ---------------------- */
        case GAME_STATE_GAME_IN_PROGRESS:
            if (controller_status == SNES_CONTROLLER_STATE_CHANGE) {
                if (snes_controller.buttons_state & SNES_BUTTON_A) {
                    tetrimino_status = tetrimino_rotate(&tetrimino, ROTATE_CW);
                } else if (snes_controller.buttons_state & SNES_BUTTON_B) {
                    tetrimino_status = tetrimino_rotate(&tetrimino, ROTATE_CCW);
                }
#if DEBUG_OUTPUT
                snes_controller_print(&snes_controller);
                if (tetrimino_status == TETRIMINO_REFRESH && snes_controller.buttons_state) {
                    tetrimino_debug_print(&tetrimino);
                }
#endif
            }

            // TODO: Check timer for game speed

            // TODO: Process input

            // TODO: Update tetromino rotation

            // TODO: Update tetromino position

            // TODO: Check for collision

            // TODO: Check for line clear

            // TODO: Check for topout condition

            // TODO: Is tetrimino locked in place?

            // TODO: Get the next tetrimino from the RNG

            // TODO: Render matrix and update LED grid

            // TODO: Update UI

            break;

            /* ------------------------- PAUSE MENU ------------------------ */
        case GAME_STATE_PAUSE:
            // TODO: Display pause menu
            break;

            /* -------------------------- GAME OVER ------------------------ */
        case GAME_STATE_GAME_ENDED:
            // TODO: Display game over screen
            break;

            /* ------------------------ HIGH SCORES ------------------------ */
        case GAME_STATE_HIGH_SCORE:
            // TODO: Display high scores
            break;

            /* ------------------------ SETTINGS MENU ---------------------- */
        case GAME_STATE_SETTINGS:
            // TODO: Display settings menu
            break;

            /* ------------------------ TEST FEATURE ------------------------ */
        case GAME_STATE_TEST_FEATURE:
            /* Developer test code START */

            /* Developer test code END */
            break;

            /* ----------------------- UNKNOWN STATES ---------------------- */
        default:
            // TODO: Handle unknown states (fail-safe)
            break;
        } // end switch

        game_loop_counter++;
        osThreadYield();

    } // end for loop
} // end game_loop

