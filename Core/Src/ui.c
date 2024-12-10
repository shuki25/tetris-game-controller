/**
  ******************************************************************************
  * @file           : ui.c
  * @author         : Ragde Primero, Dr. Joshua Butler
  * @date           : Dec 9, 2024
  * @brief          : OLED Display UI functions
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

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "ui.h"

/**
 * @brief  Initialize OLED display
 * @param  None
 * @retval None
 */
void ui_init() {
    // TODO: Initialize OLED display
}

/**
 * @brief  Display main menu selection
 * @param  None
 * @retval Menu selection (use enum constants for each menu item)
 */
void ui_main_menu_selection(void) {
    // TODO: Display main menu selection
}

/**
 * @brief  Display game progress
 * @param  None
 * @retval None
 */
void ui_game_progress() {
    // TODO: Display lines cleared, game score, level, tetrimino count and time elapsed
}

/**
 * @brief  Display game over screen
 * @param  None
 * @retval None
 */
void ui_game_over_screen() {
    // TODO: Display game over screen with final score, level, and time

    // TODO: Display high score if applicable, and prompt for name entry

}

/**
 * @brief  Display settings menu
 * @param  None
 * @retval None
 */
void ui_settings_menu() {
    // TODO: Display settings menu (brightness, reset high score, scoreboard device identifier, etc.)
}

/**
 * @brief  Display Frames Per Second (FPS) in lower left corner
 * @param  None
 * @retval None
 */
void ui_display_fps() {
    // TODO: Calculate and display frames per second
}
