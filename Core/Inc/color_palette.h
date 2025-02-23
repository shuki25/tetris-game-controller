/**
 ******************************************************************************
 * @file           : color_palette.c
 * @author         : Yoel Buzgalo, Dr. Joshua Butler
 * @date           : Feb 22, 2025
 * @brief          : Color palette for Tetris shapes
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

#ifndef INC_COLOR_PALETTE_H_
#define INC_COLOR_PALETTE_H_

#include <stdint.h>
#define COLOR_PALETTE_ROTATIONS (10)
#define COLOR_PALETTES (3)

typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} color_t;

extern color_t color_lookup_table[COLOR_PALETTE_ROTATIONS][COLOR_PALETTES];  // Declare but don't define

color_t get_color_palette(int current_level, int shape_id);

#endif /* INC_COLOR_PALETTE_H_ */
