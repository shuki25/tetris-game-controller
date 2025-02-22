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
#define RENDERER_OFFSET_X (1)
#define RENDERER_OFFSET_Y (1)
#define COLOR_PALETTE_ROTATIONS (10)
#define COLOR_PALETTES (3)

struct {
    uint8_t RED;
    uint8_t GREEN;
    uint8_t BLUE;
} typedef color_t;

extern uint16_t lookup_table[MATRIX_HEIGHT][MATRIX_WIDTH];
color_t color_lookup_table[COLOR_PALETTE_ROTATIONS][COLOR_PALETTES] = {
        {{251, 251, 251}, {74, 173, 251}, {49, 58, 246}},  // level 0: white, blue, dark blue
        {{251, 251, 251}, {172, 252, 21}, {50, 156, 4}},  // level 1: white, green, dark green
        {{251, 251, 251}, {239, 88, 246}, {204, 0, 192}},  // level 2: white, magenta, dark magenta
        {{251, 251, 251}, {76, 213, 66}, {49, 58, 246}},  // level 3: white, lime, dark lime
        {{251, 251, 251}, {85, 252, 135}, {218, 0, 71}},  // level 4: white, cyan, dark cyan
        {{251, 251, 251}, {86, 111, 251}, {85, 252, 135}},  // level 5: white, violet, lime
        {{251, 251, 251}, {105, 105, 105}, {230, 32, 8}},  // level 6: white, gray, red
        {{251, 251, 251}, {149, 0, 25}, {84, 33, 251}}, // level 7: white, red, purple
        {{251, 251, 251}, {230, 32, 8}, {49, 58, 246}},  // level 8: white, red, dark blue
        {{251, 251, 251}, {237, 143, 53}, {230, 32, 8}} // level 9: white, orange, red
};

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
	uint32_t delay_length; // in microseconds (determines refresh rate)
	uint32_t rendering_time; // in microseconds (measures actual rendering time)
	uint16_t led_position;
	uint16_t num_leds;
	matrix_t *matrix;
	led_t *led;

} renderer_t;

uint8_t generate_lookup_table();

renderer_status_t renderer_create_boundary(renderer_t *renderer);

// TODO: Function prototypes for matrix rendering functions (e.g. matrix_rendering_init, matrix_rendering_render)

renderer_status_t renderer_init(renderer_t *renderer,
		uint16_t lookup_table[MATRIX_HEIGHT][MATRIX_WIDTH], matrix_t *matrix,
		led_t *led, TIM_HandleTypeDef *htim, const uint32_t channel,
		uint32_t delay_length);
renderer_status_t renderer_render(renderer_t *renderer, matrix_t *matrix);
renderer_status_t renderer_test_render(renderer_t *renderer);
#endif /* INC_RENDERER_H_ */
