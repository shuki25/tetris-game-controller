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

#include <string.h>
#include "tetris.h"
#include "tetrimino.h"

/**
 * @brief  Calculate the score based on the number of lines cleared and the current level
 * @param  level, lines_cleared
 * @retval score
 */
uint32_t tetris_calculate_score(uint8_t lines_cleared, uint8_t level) {
    // Calculate the score based on the number of lines cleared and the current level
    switch (lines_cleared) {
    case 1:
        return (level * 40) + 40;
    case 2:
        return (level * 100) + 100;
    case 3:
        return (level * 300) + 300;
    case 4:
        return (level * 1200) + 1200;
    default:
        return 0;
    }
}

/**
 * @brief  Resets game statistics
 * @param  stats: pointer to tetris_statistics_t struct
 * @retval None
 */
void tetris_statistics_reset(tetris_statistics_t *stats) {
    memset(stats, 0, sizeof(tetris_statistics_t));
}

/**
 * @brief  Update game statistics
 * @param  current tetrimino, lines cleared, score, level
 * @retval status
 */
void tetris_update_statistics(void) {
    // TODO: Update game statistics (lines cleared, score, level, tetrimino frequency count, etc.)
}

