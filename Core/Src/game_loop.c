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

#include "main.h"
#include "stdint.h"

#include "snes_controller.h"
#include "tetrimino.h"
#include "tetrimino_shape.h"
#include "game_loop.h"
#include "itm_debug.h"
#include "util.h"
#include "cmsis_os.h"

uint32_t game_loop_counter = 0;
snes_controller_t snes_controller;

/**
 * @brief  Initialize game state
 * @param  None
 * @retval None
 */
void game_init(void) {
    // TODO: Initialize game state (structs, bitboards, etc.)
}

/**
 * @brief  Main game loop for Classic Tetris on LED Grid
 * @param  None
 * @retval None
 */
void game_loop(void) {
    snes_controller_status_t controller_status;
    char output_buffer[80];
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

    for (;;) {
        // TODO: Respond to scoreboard requests

        // TODO: Poll SNES controller
        controller_status = snes_controller_read(&snes_controller);
        if (controller_status == SNES_CONTROLLER_NO_STATE_CHANGE) {
            game_loop_counter++;
        }
        if (controller_status == SNES_CONTROLLER_STATE_CHANGE) {
#if DEBUG_OUTPUT
            snes_controller_print(&snes_controller);
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
        game_loop_counter++;
        osThreadYield();
    }
}
