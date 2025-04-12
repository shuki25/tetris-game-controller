/**
 ******************************************************************************
 * @file           : tetris.h
 * @author         : josh
 * @date           : Dec 9, 2024
 * @brief          :
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

#ifndef INC_TETRIS_H_
#define INC_TETRIS_H_

#include "tetrimino.h"

// Typedef for tetris statistics struct (e.g. tetris_statistics_t)
typedef struct {
    uint32_t score;
    uint8_t level;
    uint16_t lines_cleared;
    uint16_t tetriminos_spawned;
    uint16_t tetriminos_frequency[TETRIMINO_COUNT]; // 7 tetrimino types
} tetris_statistics_t;

// Function prototypes
uint32_t tetris_calculate_score(uint8_t lines_cleared, uint8_t level);
void tetris_statistics_reset(tetris_statistics_t *stats);

#endif /* INC_TETRIS_H_ */
