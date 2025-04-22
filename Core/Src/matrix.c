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

#include "main.h"
#include "matrix.h"
#include "itm_debug.h"
#include <string.h>
#include "tetrimino_shape.h"
#include "util.h"
#include <stdint.h>
#include "color_palette.h"

//@formatter:off
const uint16_t line_clear_mask[] = {
    0b0000000000000000,  // 0x0000
    0b0001000000001000, // 0x1008
    0b0001100000011000, // 0x1818
    0b0001110000111000, // 0x1C38
    0b0001111001111000 // 0x1E78
};
//@formatter:on

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
 * @brief  Reset matrix object
 * @param  matrix object
 * @retval None
 */
matrix_status_t matrix_clear(matrix_t *matrix) {
    for (int i = 0; i < MATRIX_DATA_SIZE; i++) {
        matrix->playfield[i] = 0;
        matrix->stack[i] = 0;
        matrix->palette1[i] = 0;
        matrix->palette2[i] = 0;
    }
    matrix->tetris_flag = 0;
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

    if (tetrimino->y >= PLAYING_FIELD_HEIGHT && tetrimino->x > PLAYING_FIELD_WIDTH - TETRIMINO_CENTER_X) {
        return MATRIX_OUT_OF_BOUNDS;
    }

    if (tetrimino->y >= PLAYING_FIELD_HEIGHT && tetrimino->x < TETRIMINO_CENTER_X) {
        return MATRIX_OUT_OF_BOUNDS;
    }

    shape_offset = tetrimino->shape_offset;
    row_index = tetrimino->y + TETRIMINO_CENTER_Y;

    // Check if tetrimino has reached beyond the bottom of the matrix
    for (int i = 0; i < TETRIMINO_BLOCK_SIZE; i++) {
        // When row_index becomes "negative", it rolls over to 255, it will be greater than PLAYING_FIELD
        if (tetrimino_shape[shape_offset + i] && row_index >= PLAYING_FIELD_HEIGHT + 10) {
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
                return MATRIX_WALL_COLLISION;
            }
            working_playfield |= row_bitmap << 16;
        } else { // If even, store in LSB
            if (working_playfield & row_bitmap) {
                return MATRIX_WALL_COLLISION;
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
}

/**
 * @brief  Move tetrimino
 * @param  tetrimino object, matrix object, direction, palette object
 * @retval status
 */
matrix_status_t matrix_move_tetrimino(matrix_t *matrix, tetrimino_t *tetrimino,
        tetrimino_move_direction_t direction) {
    tetrimino_t temp_tetrimino;
    matrix_t temp_matrix;
    matrix_status_t matrix_status;

    tetrimino_copy(&temp_tetrimino, tetrimino);
    matrix_copy(&temp_matrix, matrix);

    // Move tetrimino object in specified direction
    switch (direction) {
    case MOVE_RIGHT:
        temp_tetrimino.x++;
        if (temp_tetrimino.x >= PLAYING_FIELD_WIDTH) {
            temp_tetrimino.x = PLAYING_FIELD_WIDTH - 1;
        }
        break;
    case MOVE_LEFT:
        temp_tetrimino.x--;
        if (temp_tetrimino.x > PLAYING_FIELD_WIDTH) {
            temp_tetrimino.x = 0;
        }
        break;
    case MOVE_DOWN:
        temp_tetrimino.y--;
        if (temp_tetrimino.y > 0) {
        }
        break;
    case MOVE_UP:
        temp_tetrimino.y++;
        if (temp_tetrimino.y >= PLAYING_FIELD_HEIGHT + TETRIMINO_CENTER_Y) {
            temp_tetrimino.y = PLAYING_FIELD_HEIGHT + TETRIMINO_CENTER_Y - 1;
        }
        break;
    default:
        return MATRIX_ERROR;
        break;
    }

    matrix_status = matrix_add_tetrimino(&temp_matrix, &temp_tetrimino);
    if (matrix_status == MATRIX_WALL_COLLISION) {
        return MATRIX_NO_CHANGE;
    } else if (matrix_status == MATRIX_OUT_OF_BOUNDS) {
        return MATRIX_NO_CHANGE;
    }

    // check for collision
    matrix_status = matrix_check_collision(&temp_matrix, &temp_tetrimino);
    if (matrix_status == MATRIX_STACK_COLLISION) {
        return MATRIX_NO_CHANGE;
    }
    // update bitboard matrix object
    matrix_copy(matrix, &temp_matrix);
    tetrimino_copy(tetrimino, &temp_tetrimino);

    return MATRIX_REFRESH;
}

/**
 * @brief  Check for collision between tetrimino, boundaries, and fallen blocks
 * @param  bitboards
 * @retval True if collision, false otherwise
 */
matrix_status_t matrix_check_collision(matrix_t *matrix, tetrimino_t *tetrimino) {
    uint8_t row_index;
    uint32_t working_playfield = 0;
    uint32_t working_stack = 0;

    for (row_index = 0; row_index < PLAYING_FIELD_HEIGHT / 2; row_index++) {
        working_stack = matrix->stack[row_index];
        working_playfield = matrix->playfield[row_index];
        if (working_stack & working_playfield) {
            return MATRIX_STACK_COLLISION;
        }
    }
    return MATRIX_OK;
}

/**
 * @brief  Check for line clear (full row)
 * @param  bitboards
 * @retval Returns which rows are marked for line clear by bit position
 */
uint32_t matrix_check_line_clear(matrix_t *matrix) {
    uint32_t stack;
    uint32_t line_clear = 0; // Covers 20 rows in the playfield

    for (int row = 0; row < PLAYING_FIELD_HEIGHT; row++) {
        stack = matrix->stack[row / 2]; // Get stack value for the row
        if (row % 2 == 0) { // Even row (LSB)
            if ((stack & PLAYING_FIELD_FILLED_ROW_MASK) == PLAYING_FIELD_FILLED_ROW_MASK) {
                line_clear |= (1 << row);  // Mark row as full
            }
        } else { // Odd row (MSB)
            stack = stack >> 16; // Shift to get MSB
            if ((stack & PLAYING_FIELD_FILLED_ROW_MASK) == PLAYING_FIELD_FILLED_ROW_MASK) {
                line_clear |= (1 << row); // Mark row as full
            }
        }
    }
    return line_clear; // Return which rows are full
}

/**
 * @brief  Start line clear animation
 * @param  matrix animation object, delay
 * @retval None
 */
void matrix_line_clear_start(matrix_t *matrix, uint32_t delay) {
    memset(&matrix->animation, 0, sizeof(matrix_animation_t));
    matrix->animation.frame_nbr = CLEAR_LINE_NUM_FRAMES - 1;
    matrix->animation.animation_timer_delay = delay;
    matrix->animation.animation_timer_start = TIM2->CNT;
}

/**
 * @brief  Animate line clear
 * @param  matrix_t, line_clear bitmap
 * @retval Returns status of the operation, true if line clear is complete
 */
uint8_t matrix_line_clear_animate(matrix_t *matrix, uint32_t line_clear) {

    uint32_t working_stack_row;
    uint32_t working_stack_mask;
    uint32_t working_palette1_row;
    uint32_t working_palette2_row;

    // Check if line clear is complete and return true
    if (!line_clear) {
        return 1;
    }

    if (util_time_expired_delay(matrix->animation.animation_timer_start,
            matrix->animation.animation_timer_delay)) {
        for (int i = 0; i < PLAYING_FIELD_HEIGHT; i++) {
            working_stack_row = matrix->stack[i / 2];
            working_palette1_row = matrix->palette1[i / 2];
            working_palette2_row = matrix->palette2[i / 2];
            if (line_clear & (1 << i)) {
                if (i % 2 == 0) { // Even row (LSB)
                    working_stack_mask = 0xFFFF0000 | line_clear_mask[matrix->animation.frame_nbr]; // mask for even row and retain MSB
                    working_stack_row &= working_stack_mask;
                    working_palette1_row &= working_stack_mask;
                    working_palette2_row &= working_stack_mask;
                } else { // Odd row (MSB)
                    working_stack_mask = line_clear_mask[matrix->animation.frame_nbr] << 16 | 0xFFFF; // mask for odd row and retain LSB
                    working_stack_row &= working_stack_mask;
                    working_palette1_row &= working_stack_mask;
                    working_palette2_row &= working_stack_mask;
                }
                matrix->stack[i / 2] = working_stack_row;
                matrix->palette1[i / 2] = working_palette1_row;
                matrix->palette2[i / 2] = working_palette2_row;
            }
        }

        if (matrix->animation.frame_nbr == 0) { // Final frame, line clear animation is complete
            return 1;
        }

        matrix->animation.frame_nbr--;
        matrix->animation.animation_timer_start = TIM2->CNT;
    }
    return 0;
}

/**
 * @brief  merge tetrimino to stack
 * @param  matrix_t
 * @retval None
 */

matrix_status_t merge_with_stack(matrix_t *matrix, tetrimino_t *tetrimino) {
    uint32_t working_stack_row;
    uint32_t working_playfield_row;
    uint32_t working_palette1_row;
    uint32_t working_palette2_row;
    matrix_t temp;

    int shape_id_index = tetrimino->piece % COLOR_PALETTES;

    matrix_copy(&temp, matrix);
    for (int i = 0; i < PLAYING_FIELD_HEIGHT / 2; i++) {
        working_stack_row = matrix->stack[i];
        working_palette1_row = matrix->palette1[i];
        working_palette2_row = matrix->palette2[i];
        working_playfield_row = matrix->playfield[i];
        temp.stack[i] = working_stack_row | (working_playfield_row & PLAYING_FIELD_MASK);
        if (shape_id_index == 1) {
            temp.palette1[i] = working_palette1_row | (working_playfield_row & PLAYING_FIELD_MASK);
        }

        if (shape_id_index == 2) {
            temp.palette2[i] = working_palette2_row | (working_playfield_row & PLAYING_FIELD_MASK);
        }
    }
    matrix_copy(matrix, &temp);
    return MATRIX_OK;
}

uint32_t matrix_find_empty_row(matrix_t *matrix) {
    uint32_t empty_bitmap = 0;
    uint32_t stack_row;

    for (int row = 0; row < PLAYING_FIELD_HEIGHT; row++) {
        stack_row = matrix->stack[row / 2] & PLAYING_FIELD_MASK_BOUNDARY; // Filter out boundary bits
        if (row % 2 == 0) { // Even row (LSB)
            if ((stack_row & 0x0000FFFF) == 0) {
                empty_bitmap |= (1 << row);
            }
        } else { // Odd row (MSB)
            if ((stack_row & 0xFFFF0000) == 0) {
                empty_bitmap |= (1 << row);
            }
        }
    }

    return empty_bitmap;
}

/**
 * @brief  Reposition fallen blocks after line clear
 * @param  matrix_t, line_clear bitmap
 * @retval None
 */
matrix_status_t matrix_reposition_blocks(matrix_t *matrix, uint32_t line_clear) {
    matrix_t temp_matrix;
    uint32_t bitmap = line_clear;
    uint32_t carry, lsb, msb;
    uint8_t done = 0, occupied_bitmap = 0, skip_front_row = 0, row_index = 0;

    matrix_copy(&temp_matrix, matrix);

    while (!done) {

        carry = 0;

        // Shift all blocks in the temporary stack matrix down by one row
        for (int i = MATRIX_DATA_SIZE - 1; i >= 0; i--) {
            lsb = matrix->stack[i] & 0x0000FFFF;
            msb = matrix->stack[i] & 0xFFFF0000;
            temp_matrix.stack[i] = (msb >> 16) | carry;
            carry = lsb << 16;
        }

        // Shift all blocks in the temporary palette1 matrix down by one row
        carry = 0;
        for (int i = MATRIX_DATA_SIZE - 1; i >= 0; i--) {
            lsb = matrix->palette1[i] & 0x0000FFFF;
            msb = matrix->palette1[i] & 0xFFFF0000;
            temp_matrix.palette1[i] = (msb >> 16) | carry;
            carry = lsb << 16;
        }

        // Shift all blocks in the temporary palette2 matrix down by one row
        carry = 0;
        for (int i = MATRIX_DATA_SIZE - 1; i >= 0; i--) {
            lsb = matrix->palette2[i] & 0x0000FFFF;
            msb = matrix->palette2[i] & 0xFFFF0000;
            temp_matrix.palette2[i] = (msb >> 16) | carry;
            carry = lsb << 16;
        }

        // Merge the original matrix with the temporary matrix from the bottom up to the first empty row
        // This is done to avoid overwriting the original matrix
        skip_front_row = 1;

        for (int i = 0; i < PLAYING_FIELD_HEIGHT; i++) {
            row_index = i / 2;
            if (!(bitmap & (1 << i))) {
                skip_front_row = 0;
                if (i % 2 == 0) { // Even row (LSB)
                    temp_matrix.stack[row_index] = (temp_matrix.stack[row_index] & 0xFFFF0000)
                            | (matrix->stack[row_index] & 0x0000FFFF);
                    temp_matrix.palette1[row_index] = (temp_matrix.palette1[row_index] & 0xFFFF0000)
                            | (matrix->palette1[row_index] & 0x0000FFFF);
                    temp_matrix.palette2[row_index] = (temp_matrix.palette2[row_index] & 0xFFFF0000)
                            | (matrix->palette2[row_index] & 0x0000FFFF);
                } else { // Odd row (MSB)
                    temp_matrix.stack[row_index] = (temp_matrix.stack[row_index] & 0x0000FFFF)
                            | (matrix->stack[row_index] & 0xFFFF0000);
                    temp_matrix.palette1[row_index] = (temp_matrix.palette1[row_index] & 0x0000FFFF)
                            | (matrix->palette1[row_index] & 0xFFFF0000);
                    temp_matrix.palette2[row_index] = (temp_matrix.palette2[row_index] & 0x0000FFFF)
                            | (matrix->palette2[row_index] & 0xFFFF0000);
                }
            } else if (skip_front_row || (!skip_front_row && (bitmap & (1 << i)))) {
                break;
            }
        }

        // Update the stack matrix with the shifted blocks
        matrix_copy(matrix, &temp_matrix);

        // Update the bitmap for the next iteration
        bitmap = matrix_find_empty_row(matrix);

        // Check if there are no more cleared rows to be repositioned
        occupied_bitmap = 0;
        done = 1;

        for (int i = 0; i < PLAYING_FIELD_HEIGHT; i++) {
            if (bitmap & (1 << i)) {
                occupied_bitmap = 1;
            }
            if (occupied_bitmap && !(bitmap & (1 << i))) {
                done = 0;
                break;
            }
        }
    }

    return MATRIX_REFRESH;
}

/**
 * @brief  Copy matrix data
 * @param  destination matrix, source matrix
 * @retval None
 */
void matrix_copy(matrix_t *dest, matrix_t *src) {
    dest->height = src->height;
    dest->width = src->width;
    for (int i = 0; i < MATRIX_DATA_SIZE; i++) {
        dest->playfield[i] = src->playfield[i];
        dest->stack[i] = src->stack[i];
        dest->palette1[i] = src->palette1[i];
        dest->palette2[i] = src->palette2[i];
    }
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

