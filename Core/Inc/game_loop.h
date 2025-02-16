/**
 ******************************************************************************
 * @file           : game_loop.h
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

#ifndef INC_GAME_LOOP_H_
#define INC_GAME_LOOP_H_

#include <stdint.h>
#include "main.h"

// Game loop struct definitions

typedef enum {
    GAME_OK = 0, GAME_ERROR
} game_status_t;

typedef enum {
    GAME_STATE_SPLASH = 0,
    GAME_STATE_SPLASH_WAIT,
    GAME_STATE_MENU,
    GAME_STATE_PLAY_MENU,
    GAME_STATE_PREPARE_GAME,
    GAME_STATE_GAME_IN_PROGRESS,
    GAME_STATE_PAUSE,
    GAME_STATE_GAME_ENDED,
    GAME_STATE_HIGH_SCORE,
    GAME_STATE_SETTINGS,
    GAME_STATE_TEST_FEATURE
} game_state_t;

typedef enum {
    PLAY_STATE_NOT_STARTED = 0,
    PLAY_STATE_NORMAL,
    PLAY_STATE_HALF_SECOND_B4_LOCK,
    PLAY_STATE_LOCKED,
    PLAY_STATE_LINE_CLEAR,
    PLAY_STATE_NEXT_TETRIMINO,
    PLAY_STATE_TOP_OUT
} play_state_t;

typedef struct {
    game_state_t state;
    play_state_t play_state;
    uint32_t score;
    uint32_t level;
    uint32_t lines;
    uint32_t game_speed;
    uint32_t drop_time_delay; // in microseconds (determines drop speed)
    uint32_t drop_time_start;
    uint32_t lock_time_delay; // in microseconds (determines lock length)
    uint32_t lock_time_start;
    uint32_t line_clear_time_delay; // in microseconds (determines line clear speed)
    uint32_t line_clear_time_start;
} game_t;

// Game loop function prototypes
game_status_t game_init(void);
void game_loop(void);

#endif /* INC_GAME_LOOP_H_ */
