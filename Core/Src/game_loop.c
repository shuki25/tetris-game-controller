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

#include "game_loop.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "ws2812.h"
#include "matrix_rendering.h"

uint8_t update_screen_flag;
led_t led;
uint8_t *brightness_lookup = NULL;

void splash(){
    update_screen_flag = 0;
    ssd1306_Init();
    ssd1306_Fill(Black);
    ssd1306_WriteString("RITetris", Font_16x26, White); // name undecided
    ssd1306_SetCursor(3, 28);
    ssd1306_UpdateScreen();
}

/**
 * @brief  Initialize game state
 * @param  None
 * @retval None
 */
void game_init() {
    splash();
    matrix_rendering_init();
    // TODO: Initialize game state (structs, bitboards, etc.)
}

/**
 * @brief  Main game loop for Classic Tetris on LED Grid
 * @param  None
 * @retval None
 */
void game_loop(void) {

    // TODO: Load settings from EEPROM

    // TODO: Load high scores from EEPROM

    // TODO: Initialize game variables

    // TODO: Initialize game components

    // TODO: Start the main game loop

    for (;;) {
        // TODO: Respond to scoreboard requests

        // TODO: Poll SNES controller

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

    }
}
