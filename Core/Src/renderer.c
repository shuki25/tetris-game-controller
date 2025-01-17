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

uint8_t generate_lookup_table(
        uint16_t lookup_table[MATRIX_HEIGHT][MATRIX_WIDTH]) {
    uint16_t led_id = 0;

    for (int i = 0; i < MATRIX_HEIGHT; i++) {
        if (i % 2 == 0) {
            for (int j = 0; j < MATRIX_WIDTH; j++) {
                lookup_table[i][j] = led_id;
#if DEBUG_OUTPUT
                printf("[%d][%d]: %d ", i, j, lookup_table[i][j]);
#endif
                led_id++;
            }
        } else {
            for (int j = MATRIX_WIDTH; j > 0; j--) {
                lookup_table[i][j] = led_id;
#if DEBUG_OUTPUT
                printf("[%d][%d]: %d ", i, j, lookup_table[i][j]);
#endif
                led_id++;
            }
        }
#if DEBUG_OUTPUT
        printf("\n");
#endif
    }
    return 0;
}

WS2812_error_t led_error;
/**
 * @brief  Initialize WS2812 LED matrix
 * @param  None
 * @retval None
 */
renderer_status_t renderer_init(renderer_t *renderer,
        uint16_t lookup_table[MATRIX_HEIGHT][MATRIX_WIDTH], matrix_t *matrix,
        led_t *led, TIM_HandleTypeDef *htim, const uint32_t channel,
        uint32_t delay_length) {
    // TODO: Initialize WS2812 LED matrix

    memset(led, 0, sizeof(led_t));
    renderer->matrix = matrix;
    renderer->led = led;
    renderer->num_leds = (matrix->height * matrix->width);
    renderer->delay_length = delay_length ? delay_length : 1000000 / 10; // default to 10 Hz

    led_error = WS2812_init(renderer->led, htim, channel, htim->Init.Period,
            renderer->num_leds, 0);
    if (led_error != WS2812_OK) {
        return RENDERER_WS2812_ERROR;
    }

    renderer->led->data_sent_flag = 1;
    renderer->time_last_sent = TIM2->CNT;
    renderer->next_update_time = renderer->time_last_sent
            + renderer->delay_length;

    if (generate_lookup_table(lookup_table) != 0) {
        return RENDERER_ERROR;
    }

    return RENDERER_OK;
}

renderer_status_t renderer_create_boundary(renderer_t *renderer, uint16_t lookup_table[MATRIX_HEIGHT][MATRIX_WIDTH]){

    // NOTE: i is rows, j is columns by convention

    // render the top and bottom boundary line
    for(int j=0; j < MATRIX_WIDTH; j++){
        uint16_t led_bottom_num = lookup_table[0][j];
        uint16_t led_top_num = lookup_table[MATRIX_HEIGHT-1][j];
        WS2812_set_LED(renderer->led, led_bottom_num, 0, 0, 64);
        WS2812_set_LED(renderer->led, led_top_num, 0, 0, 64);
    }

    // render the left and right boundary lines
    for(int i=0; i < MATRIX_HEIGHT; i++){
        uint16_t led_left_side_num = lookup_table[i][0];
        uint16_t led_right_side_num = lookup_table[i][MATRIX_WIDTH-1];
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
void renderer_render(void) {
    // TODO: Send final matrix to WS2812 LED matrix
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
