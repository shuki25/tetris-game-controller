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

// TODO: Typedef for matrix status in enum (e.g. MATRIX_OK, MATRIX_ERROR)
typedef enum {
	MATRIX_OK = 0, MATRIX_ERROR
} matrix_status_t;

// TODO: Defines for matrix dimensions (e.g. MATRIX_WIDTH, MATRIX_HEIGHT)
#define MATRIX_HEIGHT (32)
#define MATRIX_WIDTH (16)

// TODO: Typedef for matrix struct (e.g. matrix_t)
typedef struct {
	uint8_t height;
	uint8_t width;
} matrix_t;

matrix_status_t matrix_init();
// TODO: Function prototypes for matrix functions (e.g. matrix_init, matrix_flatten, matrix_clear, matrix_check_collision)

#endif /* INC_MATRIX_H_ */
