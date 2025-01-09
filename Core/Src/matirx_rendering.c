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
#include "main.h"
#include "matrix_rendering.h"

WS2812_error_t led_error;

/**
 * @brief  Initialize WS2812 LED matrix
 * @param  None
 * @retval None
 */
matrix_rendering_status_t matrix_rendering_init(led_matrix_t *matrix, led_t *led, TIM_HandleTypeDef *htim,
        const uint32_t channel, uint32_t delay_length, uint16_t num_leds) {
    // TODO: Initialize WS2812 LED matrix

    memset(led, 0, sizeof(led_t));
    memset(matrix, 0, sizeof(led_matrix_t));
    matrix->led = led;
    matrix->num_leds = num_leds;
    matrix->delay_length = delay_length ? delay_length : 1000000 / 10; // default to 10 Hz

    led_error = WS2812_init(matrix->led, htim, channel, htim->Init.Period, matrix->num_leds, 0);
    if (led_error != WS2812_OK) {
        return MATRIX_RENDERING_WS2812_ERROR;
    }

    matrix->led->data_sent_flag = 1;
    matrix->time_last_sent = TIM2->CNT;
    matrix->next_update_time = matrix->time_last_sent + matrix->delay_length;

    return MATRIX_RENDERING_OK;
}

/**
 * @brief  Render final matrix to WS2812 LED matrix
 * @param  None
 * @retval None
 */
void matrix_rendering_render(void) {
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
void matrix_rendering_clear(void) {
    // TODO: Clear WS2812 LED matrix
}

/**
 * @brief  Show next tetrimino on WS2812 LED matrix
 * @param  tetrimino identifier
 * @retval None
 */
void matrix_rendering_show_next_tetrimino(void) {
    // TODO: Show next tetrimino on WS2812 LED matrix
}

matrix_rendering_status_t matrix_rendering_test_render(led_matrix_t *matrix) {

    if (TIM2->CNT < matrix->next_update_time) {
        return MATRIX_RENDERING_NOT_READY;
    }

    WS2812_clear(matrix->led);
    WS2812_set_LED(matrix->led, matrix->led_position, 0, 0, 64);
    WS2812_send(matrix->led);

    matrix->led_position++;
    if (matrix->led_position >= matrix->num_leds) {
        matrix->led_position = 0;
    }

    matrix->next_update_time = TIM2->CNT + matrix->delay_length;

    return MATRIX_RENDERING_UPDATED;

}
