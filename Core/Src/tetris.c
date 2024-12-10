/**
  ******************************************************************************
  * @file           : tetris.c
  * @author         : Dr. Joshua Butler
  * @date           : Dec 9, 2024
  * @brief          : Main Tetris logic for Classic Tetris on LED Grid
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

#include "tetris.h"

/**
 * @brief  Calculate the score based on the number of lines cleared and the current level
 * @param  level, lines_cleared
 * @retval score
 */
void tetris_calculate_score(void) {
    // TODO: Calculate the score based on the number of lines cleared and the current level
}

/**
 * @brief  Check if topout condition has been reached
 * @param  tetrimino object, matrix object
 * @retval game status
 */
void tetris_check_topout(void) {
    // TODO: Check if the top of the matrix has been reached

    // If the top of the matrix has been reached, then the game is over
}

/**
 * @brief  Update game statistics
 * @param  current tetrimino, lines cleared, score, level
 * @retval status
 */
void tetris_update_statsitcs(void) {
    // TODO: Update game statistics (lines cleared, score, level, tetrimino frequency count, etc.)
}
