/**
 ******************************************************************************
 * @file           : matrix.h
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

#ifndef INC_MATRIX_H_
#define INC_MATRIX_H_
#include <stdint.h>
#include "tetrimino.h"

typedef enum {
    MATRIX_OK = 0,
    MATRIX_ERROR,
    MATRIX_OUT_OF_BOUNDS,
    MATRIX_REFRESH,
    MATRIX_NO_CHANGE,
    MATRIX_COLLISION_DETECTED,
    MATRIX_WALL_COLLISION,
    MATRIX_STACK_COLLISION,
    MATRIX_REACHED_BOTTOM
} matrix_status_t;

#define MATRIX_HEIGHT (32)
#define MATRIX_WIDTH (16)

#if (MATRIX_HEIGHT < 32)
#define PLAYING_FIELD_HEIGHT (15)
#else
#define PLAYING_FIELD_HEIGHT (20)
#endif
#define PLAYING_FIELD_WIDTH (10)
#define PLAYING_FIELD_BOUNDARY_BITMAP (0xE007E007)  // boundary bitmap for boundary check
#define PLAYING_FIELD_MASK (0x1FF81FF8)  // playfield mask for rendering without boundary
#define PLAYING_FIELD_MASK_BOUNDARY (0x3FFC3FFC)  // playfield mask for rendering with boundary
#define PLAYING_FIELD_ODD_MASK (0x1FF80000)  // playfield mask for odd rows
#define PLAYING_FIELD_EVEN_MASK (0x00001FF8)  // playfield mask for even rows
#define MATRIX_DATA_SIZE (PLAYING_FIELD_HEIGHT >> 1)  // divide by 2 as each uint32_t holds 2 rows

typedef struct {
    uint8_t height;
    uint8_t width;
    uint32_t playfield[MATRIX_DATA_SIZE];
    uint32_t stack[MATRIX_DATA_SIZE];
    uint32_t palette1[MATRIX_DATA_SIZE];
    uint32_t palette2[MATRIX_DATA_SIZE];
} matrix_t;

// Function prototypes for matrix functions
matrix_status_t matrix_init();
matrix_status_t matrix_reset_playfield(matrix_t *matrix);
matrix_status_t matrix_add_tetrimino(matrix_t *matrix, tetrimino_t *tetrimino);
matrix_status_t matrix_check_collision(matrix_t *matrix, tetrimino_t *tetrimino);
void matrix_debug_print(matrix_t *matrix);

#endif /* INC_MATRIX_H_ */
