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

#include "main.h"
#include "matrix.h"
#include "string.h"
#include "tetrimino.h"
#include "tetrimino_shape.h"
#include "itm_debug.h"
#include "rng.h"

/*
 * Dropping tetriminos speed
 * The speed is based on the level of the game. The speed is determined by the
 * number of microseconds to wait before dropping the tetrimino by one row.
 *
 * NES runs at 60.0988 frames per second. the period between drops and the speed
 * is computed. https://meatfighter.com/nintendotetrisai/
 */

// @formatter:off
uint32_t tetrimino_drop_period[] = {
        799000, // 0.799 seconds (Level 0)
        715000, // 0.715 seconds (Level 1)
        632000, // 0.632 seconds (Level 2)
        549000, // .549 seconds (Level 3)
        466000, // .466 seconds (Level 4)
        383000, // .383 seconds (Level 5)
        300000, // .300 seconds (Level 6)
        216000, // .216 seconds (Level 7)
        133000, // .133 seconds (Level 8)
        100000, // .100 seconds (Level 9)
        83000,  // .083 seconds (Level 10-12)
        67000,  // .067 seconds (Level 13-15)
        50000,  // .050 seconds (Level 16-18)
        33000,  // .033 seconds (Level 19-28)
        17000,  // .017 seconds (Level 29-31)
        10000   // .010 seconds (Level 32+)
};
// @formatter:on
/**
 * @brief  Initialize tetrimino
 * @param  tetrimino object
 * @retval status
 */
tetrimino_status_t tetrimino_init(tetrimino_t *tetrimino) {
    // TODO: Initialize tetrimino object
    memset(tetrimino, 0, sizeof(tetrimino_t));
    rng_init((uint16_t) (TIM2->CNT & 0xFFFF));
    tetrimino->x = 5;
    tetrimino->y = PLAYING_FIELD_HEIGHT;
    tetrimino->piece = rng_next() % TETRIMINO_COUNT;
    tetrimino->rotation = tetrimino_spawn[tetrimino->piece];
    tetrimino->shape_offset = tetrimino_shape_offset_lut[tetrimino->piece][tetrimino->rotation];
//    tetrimino->piece = rng_next() % TETRIMINO_COUNT;
    tetrimino->next_piece = rng_next() % TETRIMINO_COUNT;

    return TETRIMINO_OK;
}

/**
 * @brief  Rotate tetrimino
 * @param  tetrimino object
 * @retval status
 */
tetrimino_status_t tetrimino_rotate(tetrimino_t *tetrimino, rotation_direction_t direction) {
    tetrimino_t temp;

    tetrimino_copy(&temp, tetrimino);

    switch (direction) {
    case ROTATE_CW:
        temp.rotation = (tetrimino->rotation + 1);
        if (temp.rotation >= TETRIMINO_ROTATION_COUNT) {
            temp.rotation = 0;
        }
        break;
    case ROTATE_CCW:
        temp.rotation = tetrimino->rotation - 1;
        if (temp.rotation >= TETRIMINO_ROTATION_COUNT) {
            temp.rotation = TETRIMINO_ROTATION_COUNT - 1;
        }
        break;
    default:
        return TETRIMINO_ERROR;
        break;
    }

    temp.shape_offset = tetrimino_shape_offset_lut[temp.piece][temp.rotation];
    tetrimino_copy(tetrimino, &temp);

    return TETRIMINO_REFRESH;
}

/**
 * @brief  Spawn next tetrimino
 * @param  tetrimino object
 * @retval status
 */
tetrimino_status_t tetrimino_next(tetrimino_t *tetrimino) {
    tetrimino->x = 5;
    tetrimino->y = PLAYING_FIELD_HEIGHT;
    tetrimino->piece = tetrimino->next_piece;
    tetrimino->rotation = tetrimino_spawn[tetrimino->piece];
    tetrimino->shape_offset = tetrimino_shape_offset_lut[tetrimino->piece][tetrimino->rotation];
//    tetrimino->piece = rng_next() % TETRIMINO_COUNT;
    tetrimino->next_piece = rng_next() % TETRIMINO_COUNT;

    return TETRIMINO_OK;
}

tetrimino_status_t tetrimino_copy(tetrimino_t *dst, tetrimino_t *src) {
    memcpy(dst, src, sizeof(tetrimino_t));
    return TETRIMINO_OK;
}

uint32_t tetrimino_drop_speed(uint8_t level) {
    if (level < 10) {
        return tetrimino_drop_period[level];
    } else if (level < 13) {
        return tetrimino_drop_period[10];
    } else if (level < 16) {
        return tetrimino_drop_period[11];
    } else if (level < 19) {
        return tetrimino_drop_period[12];
    } else if (level < 29) {
        return tetrimino_drop_period[13];
    } else if (level < 39) {
        return tetrimino_drop_period[14];
    } else {
        return tetrimino_drop_period[15];
    }
}

void tetrimino_debug_print(tetrimino_t *tetrimino) {
    uint8_t bitmap;
    printf("===================\n");
    printf("Tetrimino: %d\n", tetrimino->piece);
    printf("Rotation: %d\n", tetrimino->rotation);
    printf("Offset: %d\n", tetrimino_shape_offset_lut[tetrimino->piece][tetrimino->rotation]);
    printf("X: %d\n", tetrimino->x);
    printf("Y: %d\n", tetrimino->y);
    printf("Next: %d\n", tetrimino->next_piece);
    for (uint8_t i = 0; i < TETRIMINO_BLOCK_SIZE; i++) {
        for (uint8_t j = TETRIMINO_BLOCK_SIZE - 1; j < TETRIMINO_BLOCK_SIZE; j--) {
            bitmap = tetrimino_shape[tetrimino_shape_offset_lut[tetrimino->piece][tetrimino->rotation] + i];
            if (bitmap & (1 << j)) {
                printf("*");
            } else {
                printf("-");
            }
        }
        printf("\n");
    }
}
