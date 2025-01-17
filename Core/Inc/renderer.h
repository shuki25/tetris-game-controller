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

#ifndef INC_RENDERER_H_
#define INC_RENDERER_H_

#include <stdint.h>
#include "ws2812.h"
#include "matrix.h"

#define MAX_PLAYFIELD_HEIGHT (20)
#define MAX_PLAYFIELD_WIDTH (MATRIX_WIDTH - 5)

// TODO: Typedef for matrix rendering status in enum (e.g. MATRIX_RENDERING_OK, MATRIX_RENDERING_ERROR)
typedef enum {
	RENDERER_OK = 0,
	RENDERER_ERROR,
	RENDERER_WS2812_ERROR,
	RENDERER_NOT_READY,
	RENDERER_UPDATED
} renderer_status_t;

// TODO: Defines for LED matrix dimensions (e.g. LED_MATRIX_WIDTH, LED_MATRIX_HEIGHT)

// TODO: Typedef for LED matrix struct (e.g. led_matrix_t)
typedef struct {
	uint8_t data_sent_flag;
	uint8_t brightness;
	uint32_t time_last_sent;
	uint32_t next_update_time;
	uint32_t delay_length; // in microseconds
	uint16_t led_position;
	uint16_t num_leds;
	matrix_t *matrix;
	led_t *led;

} renderer_t;

uint8_t generate_lookup_table(
		uint16_t lookup_table[MATRIX_HEIGHT][MATRIX_WIDTH]);

renderer_status_t renderer_create_boundary(renderer_t *renderer, uint16_t lookup_table[MATRIX_HEIGHT][MATRIX_WIDTH]);

// TODO: Function prototypes for matrix rendering functions (e.g. matrix_rendering_init, matrix_rendering_render)

renderer_status_t renderer_init(renderer_t *renderer,
		uint16_t lookup_table[MATRIX_HEIGHT][MATRIX_WIDTH], matrix_t *matrix,
		led_t *led, TIM_HandleTypeDef *htim, const uint32_t channel,
		uint32_t delay_length);
renderer_status_t renderer_test_render(renderer_t *renderer);
#endif /* INC_RENDERER_H_ */
