/**
 ******************************************************************************
 * @file           : matirx_rendering.c
 * @author         : Yoel Buzgalo, Dr. Joshua Butler
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

#include <string.h>
#include "renderer.h"
#include "matrix.h"
#include "main.h"
#include "itm_debug.h"
#include "util.h"
#include "tetrimino_shape.h"


uint8_t generate_lookup_table() {
    uint16_t led_id = 0;

    for (int i = 0; i < MATRIX_HEIGHT; i++) {
        if (i % 2 == 0) {
            for (int j = 0; j < MATRIX_WIDTH; j++) {
                lookup_table[i][j] = led_id;
//#if DEBUG_OUTPUT
//                printf("[%d][%d]: %d ", i, j, lookup_table[i][j]);
//#endif
                led_id++;
            }
        } else {
            for (int j = MATRIX_WIDTH - 1; j >= 0; j--) {
                lookup_table[i][j] = led_id;
//#if DEBUG_OUTPUT
//                printf("[%d][%d]: %d ", i, j, lookup_table[i][j]);
//#endif
                led_id++;
            }
        }
//#if DEBUG_OUTPUT
//        printf("\n");
//#endif
    }
    return 0;
}

WS2812_error_t led_error;
/**
 * @brief  Initialize WS2812 LED matrix
 * @param  None
 * @retval None
 */
renderer_status_t renderer_init(renderer_t *renderer, uint16_t lookup_table[MATRIX_HEIGHT][MATRIX_WIDTH],
        matrix_t *matrix, led_t *led, TIM_HandleTypeDef *htim, const uint32_t channel, uint32_t delay_length) {
    // TODO: Initialize WS2812 LED matrix

    memset(led, 0, sizeof(led_t));
    renderer->matrix = matrix;
    renderer->led = led;
    renderer->num_leds = (matrix->height * matrix->width);
    renderer->delay_length = delay_length ? delay_length : 1000000 / 24; // default to 24 Hz

    led_error = WS2812_init(renderer->led, htim, channel, htim->Init.Period, renderer->num_leds, 0);
    if (led_error != WS2812_OK) {
        return RENDERER_WS2812_ERROR;
    }

    renderer->led->data_sent_flag = 1;
    renderer->time_last_sent = TIM2->CNT;
    renderer->next_update_time = renderer->time_last_sent + renderer->delay_length;

    if (generate_lookup_table(lookup_table) != 0) {
        return RENDERER_ERROR;
    }

    return RENDERER_OK;
}

renderer_status_t renderer_create_boundary(renderer_t *renderer) {

    // NOTE: i is rows, j is columns by convention

    // render bottom boundary line
    for (int j = 0; j < (MAX_PLAYFIELD_WIDTH); j++) {
        uint16_t led_bottom_num = lookup_table[0][j];
        WS2812_set_LED(renderer->led, led_bottom_num, 0, 0, 64);
    }

    // render the left and right boundary lines
    for (int i = 0; i <= MAX_PLAYFIELD_HEIGHT; i++) {
        uint16_t led_left_side_num = lookup_table[i][0];
        uint16_t led_right_side_num = lookup_table[i][MATRIX_WIDTH - 5];
        WS2812_set_LED(renderer->led, led_left_side_num, 0, 0, 64);
        WS2812_set_LED(renderer->led, led_right_side_num, 0, 0, 64);
    }

    WS2812_send(renderer->led);

    return RENDERER_OK;
}

/**
 * @brief  Render final matrix to WS2812 LED matrix
 * @param  None
 * @retval None
 */
renderer_status_t renderer_render(renderer_t *renderer, matrix_t *matrix, tetrimino_t *tetrimino) {

    uint32_t two_rows_bitmap = 0;
    uint32_t two_rows_stack_bitmap = 0;
    uint32_t working_playfield = 0;
    uint32_t working_stack = 0;
    uint32_t working_palette1 = 0;
    uint32_t working_palette2 = 0;
    uint32_t render_start_time = 0;
    uint32_t render_end_time = 0;
    uint16_t led_num = 0;
    uint8_t y = 0;

    // Check if it's time to update the LED matrix
    if (TIM2->CNT < renderer->next_update_time) {
        return RENDERER_NOT_READY;
    }

    render_start_time = TIM2->CNT;

    // Render tetrimino in the playfield attribute
    for (int i = 0; i < MATRIX_DATA_SIZE; i++) {
        two_rows_bitmap = matrix->playfield[i];
        two_rows_stack_bitmap = matrix->stack[i];
        // render even row
        working_playfield = two_rows_bitmap & PLAYING_FIELD_EVEN_MASK;
        working_stack = two_rows_stack_bitmap & PLAYING_FIELD_EVEN_MASK;
        working_palette1 = matrix->palette1[i] & PLAYING_FIELD_EVEN_MASK;
        working_palette2 = matrix->palette2[i] & PLAYING_FIELD_EVEN_MASK;
        working_playfield = working_playfield >> 3; // shift to remove boundary
        working_stack = working_stack >> 3; // shift to remove boundary
        working_palette1 = working_palette1 >> 3; // shift to remove boundary
        working_palette2 = working_palette2 >> 3; // shift to remove boundary
        for (int j = 0, x = PLAYING_FIELD_WIDTH + RENDERER_OFFSET_X - 1; j < PLAYING_FIELD_WIDTH; j++, x--) {
            y = (i * 2) + RENDERER_OFFSET_Y;
            led_num = lookup_table[y][x];



            if (working_playfield >> j & 1) {
                WS2812_set_LED(renderer->led, led_num, 0, 32, 0); // TODO: Set color based on palette lookup table
            } else if (working_stack >> j & 1) {
                if (working_palette1 >> j & 1) {
                    WS2812_set_LED(renderer->led, led_num, 64, 0, 0);
                } else if (working_palette2 >> j & 1) {
                    WS2812_set_LED(renderer->led, led_num, 0, 0, 32);
                } else {
                    WS2812_set_LED(renderer->led, led_num, 0, 32, 64);
                }
            } else {
                WS2812_set_LED(renderer->led, led_num, 0, 0, 0);
            }
        }
        // render odd row
        working_playfield = two_rows_bitmap & PLAYING_FIELD_ODD_MASK;
        working_stack = two_rows_stack_bitmap & PLAYING_FIELD_ODD_MASK;
        working_palette1 = matrix->palette1[i] & PLAYING_FIELD_ODD_MASK;
        working_palette2 = matrix->palette2[i] & PLAYING_FIELD_ODD_MASK;
        working_playfield = working_playfield >> 19; // shift to LSB and to remove boundary
        working_stack = working_stack >> 19; // shift to LSB and to remove boundary
        working_palette1 = working_palette1 >> 19; // shift to LSB and to remove boundary
        working_palette2 = working_palette2 >> 19; // shift to LSB and to remove boundary
        for (int j = 0, x = PLAYING_FIELD_WIDTH + RENDERER_OFFSET_X - 1; j < PLAYING_FIELD_WIDTH; j++, x--) {
            y = (i * 2) + RENDERER_OFFSET_Y;
            led_num = lookup_table[y + 1][x];
            if (working_playfield >> j & 1) {
                WS2812_set_LED(renderer->led, led_num, 0, 32, 0);
            } else if (working_stack >> j & 1) {
                if (working_palette1 >> j & 1) {
                    WS2812_set_LED(renderer->led, led_num, 64, 0, 0);
                } else if (working_palette2 >> j & 1) {
                    WS2812_set_LED(renderer->led, led_num, 0, 0, 32);
                } else {
                    WS2812_set_LED(renderer->led, led_num, 0, 32, 64);
                }
            } else {
                WS2812_set_LED(renderer->led, led_num, 0, 0, 0);
            }
        }
    }

    tetrimino_piece_t next_piece = tetrimino->next_piece;

    uint8_t shape_offset = tetrimino_shape_offset_lut[next_piece][tetrimino_preview[next_piece]];
    uint8_t bitmap = 0;
    uint8_t preview_x = 12;
    uint8_t preview_y = 14;

    for (int i = 0; i < TETRIMINO_BLOCK_SIZE; i++) {
        bitmap = tetrimino_shape[shape_offset + i];
        preview_x = 15;
        for (int j = 0; j < TETRIMINO_BLOCK_SIZE - 1; j++) {
            if (bitmap & (1 << (j + 1))) {
                WS2812_set_LED(renderer->led, lookup_table[preview_y][preview_x], 0, 0, 64);
            } else {
                WS2812_set_LED(renderer->led, lookup_table[preview_y][preview_x], 0, 0, 0);
            }
            preview_x--;
        }
        preview_y--;
    }
    // Render next tetrimino

    WS2812_send(renderer->led);

    render_end_time = TIM2->CNT;
    renderer->rendering_time = util_time_diff_us(render_start_time, render_end_time);
    // Update the next update time
    renderer->time_last_sent = TIM2->CNT;
    renderer->next_update_time = TIM2->CNT + renderer->delay_length;

    return RENDERER_UPDATED;

    //    WS2812_fill(matrix->led, 0, 0, 32);
//    WS2812_set_brightness(&led, 5);
//    WS2812_send(&led);
}

/**
 * @brief  Clear WS2812 LED matrix
 * @param  None
 * @retval None
*/
void renderer_clear(void) {
    // TODO: Clear WS2812 LED matrix
}

/**
 * @brief  Show next tetrimino on WS2812 LED matrix
 * @param  tetrimino identifier
 * @retval None
 */
void renderer_show_next_tetrimino(void) {
    // TODO: Show next tetrimino on WS2812 LED matrix
}

renderer_status_t renderer_test_render(renderer_t *renderer) {

    if (TIM2->CNT < renderer->next_update_time) {
        return RENDERER_NOT_READY;
    }

    WS2812_clear(renderer->led);
    WS2812_set_LED(renderer->led, renderer->led_position, 0, 0, 64);
    WS2812_send(renderer->led);

    renderer->led_position++;
    if (renderer->led_position >= renderer->num_leds) {
        renderer->led_position = 0;
    }

    renderer->next_update_time = TIM2->CNT + renderer->delay_length;

    return RENDERER_UPDATED;

}
