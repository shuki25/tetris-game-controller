/*
 * color_palette.h
 *
 *  Created on: Feb 22, 2025
 *      Author: yoel
 */

#ifndef INC_COLOR_PALETTE_H_
#define INC_COLOR_PALETTE_H_

#include <stdint.h>
#define COLOR_PALETTE_ROTATIONS (10)
#define COLOR_PALETTES (3)

typedef struct {
    uint8_t RED;
    uint8_t GREEN;
    uint8_t BLUE;
} color_t;

extern color_t color_lookup_table[COLOR_PALETTE_ROTATIONS][COLOR_PALETTES];  // Declare but don't define

color_t get_color_palette(int current_level, int shape_id);


#endif /* INC_COLOR_PALETTE_H_ */
