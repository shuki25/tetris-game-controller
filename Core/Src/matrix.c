/**
  ******************************************************************************
  * @file           : matrix.c
  * @author         : Yoel Buzgalo, Dr. Joshua Butler
  * @date           : Dec 9, 2024
  * @brief          : Bitboard representation of Tetris matrix
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

#include "matrix.h"
#include <string.h>

/**
 * @brief  Initialize bitboards (tetrimino, fallen blocks, palette)
 * @param  bitboards
 * @retval None
 */
matrix_status_t matrix_init(matrix_t* matrix) {
    memset(matrix, 0, sizeof(matrix_t));
    matrix->height = MATRIX_HEIGHT;
    matrix->width = MATRIX_WIDTH;
    return MATRIX_OK;
}

/**
 * @brief  Add tetrimino to matrix
 * @param  bitboards, tetrimino object, tetrimino coordinates
 * @retval None
 */
void matrix_add_tetrimino(void) {
    // TODO: Add tetrimino to matrix

    // TODO: Update palette bitboards
}

/**
 * @brief  Flatten bitboards into final matrix
 * @param  bitboards
 * @retval None
 */
void matrix_flatten(void) {
    // TODO: Flatten bitboards (tetrimino, fallen blocks, palette) into final matrix
}

/**
 * @brief  Clear fallen blocks from matrix
 * @param  None
 * @retval None
 */
void matrix_clear(void) {
    // TODO: Clear fallen blocks from matrix
}

/**
 * @brief  Check for collision between tetrimino, boundaries, and fallen blocks
 * @param  bitboards
 * @retval True if collision, false otherwise
 */
void matrix_check_collision(void) {
    // TODO: Check for collision between tetrimino, boundaries, and fallen blocks
}

/**
 * @brief  Check for line clear (full rows)
 * @param  bitboards
 * @retval True if line clear, false otherwise
 */
void matrix_check_line_clear(void) {
    // TODO: Check for line clear (full rows)
}

/**
 * @brief  Clear full rows
 * @param  bitboards
 * @retval Number of rows cleared
 */
void matrix_line_clear(void) {
    // TODO: Clear full rows

    // TODO: Reposition fallen blocks
}
