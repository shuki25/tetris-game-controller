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
#include "itm_debug.h"
#include <string.h>
#include "tetrimino_shape.h"

/**
 * @brief  Initialize bitboards (tetrimino, fallen blocks, palette)
 * @param  bitboards
 * @retval matrix status
 */
matrix_status_t matrix_init(matrix_t *matrix) {
    memset(matrix, 0, sizeof(matrix_t));
    matrix->height = MATRIX_HEIGHT;
    matrix->width = MATRIX_WIDTH;
    matrix_reset_playfield(matrix);

    return MATRIX_OK;
}

/**
 * @brief  Reset playfield to boundary bitmap
 * @param  matrix
 * @retval matrix status
 */
matrix_status_t matrix_reset_playfield(matrix_t *matrix) {
    for (int i = 0; i < MATRIX_DATA_SIZE; i++) {
        matrix->playfield[i] = PLAYING_FIELD_BOUNDARY_BITMAP;
    }
    return MATRIX_OK;
}

/**
 * @brief  Add tetrimino to matrix
 * @param  bitboards, tetrimino object, tetrimino coordinates
 * @retval None
 */

matrix_status_t matrix_add_tetrimino(matrix_t *matrix, tetrimino_t *tetrimino) {
    uint8_t shape_offset = 0;
    uint16_t row_bitmap = 0;
    uint32_t working_playfield = 0;
    uint32_t temp_playfield[MATRIX_DATA_SIZE];
    uint8_t row_index = 0;

    // Reset playfield
    matrix_reset_playfield(matrix);

    memset(temp_playfield, 0, sizeof(temp_playfield));
    for (int i = 0; i < MATRIX_DATA_SIZE; i++) {
        temp_playfield[i] = matrix->playfield[i];
    }

    // Check if tetrimino is within visible bounds
    if (tetrimino->y >= PLAYING_FIELD_HEIGHT + TETRIMINO_CENTER_Y) {
        return MATRIX_OUT_OF_BOUNDS;
    }

    shape_offset = tetrimino->shape_offset;
    row_index = tetrimino->y + TETRIMINO_CENTER_Y;

    // Check if tetrimino has reached beyond the bottom of the matrix
    for (int i = 0; i < TETRIMINO_BLOCK_SIZE; i++) {
        // When row_index becomes "negative", it rolls over to 255, it will be greater than PLAYING_FIELD
        if (tetrimino_shape[shape_offset + i] && row_index >= PLAYING_FIELD_HEIGHT) {
            return MATRIX_REACHED_BOTTOM;
        }
        row_index--;
    }

    // Superimpose tetrimino on playfield
    row_index = tetrimino->y + TETRIMINO_CENTER_Y;
    for (int i = 0; i < TETRIMINO_BLOCK_SIZE; i++) {
        if (row_index >= PLAYING_FIELD_HEIGHT) { // Check if row is within visible bounds
            row_index--;
            continue;
        }

        working_playfield = temp_playfield[row_index >> 1];

        // Get the row bitmap
        row_bitmap = (uint16_t) tetrimino_shape[shape_offset + i];
        row_bitmap = row_bitmap << 3; // shift to avoid boundary

        // Shift the row to the correct position
        if (tetrimino->x >= PLAYING_FIELD_WIDTH - TETRIMINO_CENTER_X) {
            row_bitmap = row_bitmap >> (tetrimino->x - PLAYING_FIELD_WIDTH + TETRIMINO_CENTER_X + 1);
        } else {
            row_bitmap = row_bitmap << (PLAYING_FIELD_WIDTH - (tetrimino->x + TETRIMINO_CENTER_X) - 1);
        }

        // Add the row bitmap to the playfield
        if (row_index % 2) { // If odd, store in MSB
            if (working_playfield & (row_bitmap << 16)) {
                return MATRIX_COLLISION_DETECTED;
            }
            working_playfield |= row_bitmap << 16;
        } else { // If even, store in LSB
            if (working_playfield & row_bitmap) {
                return MATRIX_COLLISION_DETECTED;
            }
            working_playfield |= row_bitmap;
        }
        temp_playfield[row_index >> 1] = working_playfield;
        row_index--;
    }

    // No collision detected, update playfield
    for (int i = 0; i < MATRIX_DATA_SIZE; i++) {
        matrix->playfield[i] = temp_playfield[i];
    }

    return MATRIX_REFRESH;

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
uint8_t matrix_check_line_clear(matrix_t *matrix) {
    // TODO: Check for line clear (full rows)
    uint8_t stack;
    uint8_t mask = 0xFFFF;
    uint8_t line_clear = 0;

    for (int row = 0; row < PLAYING_FIELD_HEIGHT; row++) {
        stack = matrix->stack[row / 2]; // Get stack value for the row
        if (row % 2 == 0) { // Even row (LSB)
            if ((stack & mask) == mask) {
                line_clear |= (1 << row);  // Mark row as full
                }
        } else { // Odd row (MSB)
            stack = stack >> 16; // Shift to get MSB
            if ((stack & mask) == mask) {
                line_clear |= (1 << row); // Mark row as full
                }
        }
    }
    return line_clear; // Return which rows are full
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

void matrix_debug_print(matrix_t *matrix) {
    uint32_t two_rows_bitmap;
    uint16_t row_bitmap;
    uint8_t row;
    printf("===================\n");
    printf("Matrix height: %d\n", matrix->height);
    printf("Matrix width: %d\n", matrix->width);
    printf("Matrix playfield (hex):\n");
    for (int i = 0; i < MATRIX_DATA_SIZE; i++) {
        printf("[%0d] 0x%08lX\n", i, matrix->playfield[i]);
    }
    printf("\n");
    for (int i = PLAYING_FIELD_HEIGHT - 1; i >= 0; i -= 2) {
        row = i >> 1; // divide by 2 to get row index
        printf("Row %02d: |", i);
        two_rows_bitmap = matrix->playfield[row];
        // print odd row
        row_bitmap = (two_rows_bitmap & PLAYING_FIELD_ODD_MASK) >> 16;
        row_bitmap = row_bitmap >> 3; // shift to remove boundary
        for (int j = PLAYING_FIELD_WIDTH - 1; j >= 0; j--) {
            if (row_bitmap >> j & 1) {
                printf("X");
            } else {
                printf(" ");
            }
        }
        printf("|\n");
        printf("Row %02d: |", i - 1);
        // print even row
        row_bitmap = (two_rows_bitmap & PLAYING_FIELD_EVEN_MASK);
        row_bitmap = row_bitmap >> 3; // shift to remove boundary
        for (int j = PLAYING_FIELD_WIDTH - 1; j >= 0; j--) {
            if (row_bitmap >> j & 1) {
                printf("X");
            } else {
                printf(" ");
            }
        }
        printf("|\n");
    }
    printf("        +==========+\n");
}

