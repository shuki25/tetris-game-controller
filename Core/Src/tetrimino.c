/**
 ******************************************************************************
 * @file           : tetrimino.c
 * @author         : Dr. Joshua Butler
 * @date           : Dec 9, 2024
 * @brief          : Tetrimino representation for Classic Tetris on LED Grid
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

#include "string.h"
#include "tetrimino.h"
#include "tetrimino_shape.h"
#include "itm_debug.h"
#include "rng.h"

/**
 * @brief  Initialize tetrimino
 * @param  tetrimino object
 * @retval status
 */
tetrimino_status_t tetrimino_init(tetrimino_t *tetrimino) {
	// TODO: Initialize tetrimino object
	memset(tetrimino, 0, sizeof(tetrimino_t));
	rng_init(0);
	tetrimino->piece = TETRIMINO_T;
	tetrimino->rotation = tetrimino_spawn[tetrimino->piece];
//    tetrimino->piece = rng_next() % TETRIMINO_COUNT;
	tetrimino->next_piece = rng_next() % TETRIMINO_COUNT;

	return TETRIMINO_OK;
}

/**
 * @brief  Rotate tetrimino
 * @param  tetrimino object
 * @retval status
 */
tetrimino_status_t tetrimino_rotate(tetrimino_t *tetrimino,
		rotation_direction_t direction) {
	tetrimino_t temp;

	// TODO: Rotate tetrimino object
	switch (direction) {
	case ROTATE_CW:
		temp.rotation = (tetrimino->rotation - 1);
		if (temp.rotation >= TETRIMINO_ROTATION_COUNT) {
			temp.rotation = TETRIMINO_ROTATION_COUNT - 1;
		}
		break;
	case ROTATE_CCW:
		temp.rotation = tetrimino->rotation + 1;
		if (temp.rotation >= TETRIMINO_ROTATION_COUNT) {
			temp.rotation = 0;
		}
		break;
	default:
		return TETRIMINO_ERROR;
		break;
	}

	// TODO: check for collision

	// TODO: update bitboard matrix object
	tetrimino_copy(tetrimino, &temp);

	return TETRIMINO_REFRESH;
}

/**
 * @brief  Move tetrimino
 * @param  tetrimino object, matrix object, direction, palette object
 * @retval status
 */
void tetrimino_move(void) {
	// TODO: Move tetrimino object in specified direction

	// TODO: check for collision

	// TODO: update bitboard matrix object
}

tetrimino_status_t tetrimino_copy(tetrimino_t *dst, tetrimino_t *src) {
	memcpy(dst, src, sizeof(tetrimino_t));
	return TETRIMINO_OK;
}

void tetrimino_debug_print(tetrimino_t *tetrimino) {
	uint8_t bitmap;
	printf("===================\n");
	printf("Tetrimino: %d\n", tetrimino->piece);
	printf("Rotation: %d\n", tetrimino->rotation);
	printf("X: %d\n", tetrimino->x);
	printf("Y: %d\n", tetrimino->y);
	printf("Next: %d\n", tetrimino->next_piece);
	for (uint8_t i = 0; i < TETRIMINO_BLOCK_SIZE; i++) {
		for (uint8_t j = 0; j < TETRIMINO_BLOCK_SIZE; j++) {
			bitmap =
					tetrimino_shape[tetrimino_shape_offset_lut[tetrimino->piece][tetrimino->rotation]
							+ i];
			if (bitmap & (1 << j)) {
				printf("*");
			} else {
				printf("-");
			}
		}
		printf("\n");
	}
}
