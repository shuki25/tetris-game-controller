/**
 ******************************************************************************
 * @file           : tetrimino.h
 * @author         : Dr. Joshua Butler
 * @date           : Dec 9, 2024
 * @brief          : Tetrimino representation for Classic Tetris on LED Grid
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

#ifndef INC_TETRIMINO_H_
#define INC_TETRIMINO_H_

#include <stdint.h>

typedef enum {
    TETRIMINO_OK = 0, TETRIMINO_ERROR, TETRIMINO_REFRESH
} tetrimino_status_t;

typedef enum {
    TETRIMINO_T = 0,
    TETRIMINO_J,
    TETRIMINO_Z,
    TETRIMINO_O,
    TETRIMINO_S,
    TETRIMINO_L,
    TETRIMINO_I,
    TETRIMINO_COUNT
} tetrimino_piece_t;

typedef enum {
    TETRIMINO_ROTATION_UP = 0,
    TETRIMINO_ROTATION_RIGHT,
    TETRIMINO_ROTATION_DOWN,
    TETRIMINO_ROTATION_LEFT,
    TETRIMINO_ROTATION_COUNT
} tetrimino_rotation_t;

typedef enum {
    ROTATE_CCW = 0, ROTATE_CW
} rotation_direction_t;

typedef struct {
    tetrimino_piece_t piece;  // Current piece
    tetrimino_rotation_t rotation;  // Current rotation position
    uint8_t x;  // Current x position (of the tetrimino center)
    uint8_t y;  // Current y position (of the tetrimino center)
    tetrimino_piece_t next_piece; // Next random generated piece
    uint8_t shape_offset; // offset pointer to the tetrimino_shape array of the current piece
} tetrimino_t;

// Function prototypes for tetrimino functions

tetrimino_status_t tetrimino_init(tetrimino_t *tetrimino);
tetrimino_status_t tetrimino_rotate(tetrimino_t *tetrimino, rotation_direction_t direction);
tetrimino_status_t tetrimino_copy(tetrimino_t *dst, tetrimino_t *src);
void tetrimino_debug_print(tetrimino_t *tetrimino);

tetrimino_status_t tetrimino_init(tetrimino_t *tetrimino);
tetrimino_status_t tetrimino_rotate(tetrimino_t *tetrimino, rotation_direction_t direction);
tetrimino_status_t tetrimino_copy(tetrimino_t *dst, tetrimino_t *src);
void tetrimino_debug_print(tetrimino_t *tetrimino);

#endif /* INC_TETRIMINO_H_ */
