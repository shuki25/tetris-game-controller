/**
 ******************************************************************************
 * @file           : matrix_rendering.h
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

#ifndef INC_MATRIX_RENDERING_H_
#define INC_MATRIX_RENDERING_H_

#include <stdint.h>
#include "ws2812.h"

// TODO: Typedef for matrix rendering status in enum (e.g. MATRIX_RENDERING_OK, MATRIX_RENDERING_ERROR)
typedef enum {
    MATRIX_RENDERING_OK = 0,
    MATRIX_RENDERING_ERROR,
    MATRIX_RENDERING_WS2812_ERROR,
    MATRIX_RENDERING_NOT_READY,
    MATRIX_RENDERING_UPDATED
} matrix_rendering_status_t;

// TODO: Defines for LED matrix dimensions (e.g. LED_MATRIX_WIDTH, LED_MATRIX_HEIGHT)

// TODO: Typedef for LED matrix struct (e.g. led_matrix_t)
typedef struct {
    uint8_t data_sent_flag;
    uint16_t num_leds;
    uint8_t brightness;
    uint32_t time_last_sent;
    uint32_t next_update_time;
    uint32_t delay_length; // in microseconds
    uint16_t led_position;
    led_t *led;

} led_matrix_t;
// TODO: Function prototypes for matrix rendering functions (e.g. matrix_rendering_init, matrix_rendering_render)

matrix_rendering_status_t matrix_rendering_init(led_matrix_t *matrix, led_t *led, TIM_HandleTypeDef *htim,
        const uint32_t channel, uint32_t delay_length, uint16_t num_leds);
matrix_rendering_status_t matrix_rendering_test_render(led_matrix_t *matrix);
#endif /* INC_MATRIX_RENDERING_H_ */
