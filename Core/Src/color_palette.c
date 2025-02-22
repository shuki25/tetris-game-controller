/*
 * color_palette.c
 *
 *  Created on: Feb 22, 2025
 *      Author: yoel
 */

#include "color_palette.h"

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

color_t get_color_palette(int current_level, int shape_id){
    int rotation_index = current_level % COLOR_PALETTE_ROTATIONS;
    int shape_id_index = shape_id % COLOR_PALETTES;

    return color_lookup_table[rotation_index][shape_id_index];
}
