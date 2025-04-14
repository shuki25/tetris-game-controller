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

#include "color_palette.h"

//@formatter:off

const color_t color_lookup_table[COLOR_PALETTE_ROTATIONS][COLOR_PALETTES] = {
    {{150, 150, 150}, {0, 50, 150}, {150, 50, 0}},         // level 0: white, medium blue, vivid orange
    {{150, 150, 150}, {200, 0, 0}, {0, 50, 150}},         // level 1: white, amber, bright blue
    {{150, 150, 150}, {150, 0, 150}, {0, 150, 100}},        // level 2: white, magenta, teal fix
    {{150, 150, 150}, {50, 50, 150}, {150, 0, 150}},        // level 3: white, cyan, purple fix
    {{150, 150, 150}, {150, 0, 150}, {0, 150, 50}},         // level 4: white, purple, bright green ok
    {{150, 150, 150}, {150, 75, 0}, {0, 75, 150}},        // level 5: white, orange, sky blue ok
    {{150, 150, 150}, {0, 100, 0}, {0, 0, 100}},      // level 6: white, olive, rose ok
    {{150, 150, 150}, {150, 50, 0}, {0, 50, 150}},          // level 7: white, brown, navy ok
    {{150, 150, 150}, {0, 50, 150}, {150, 50, 0}},          // level 8: white, sky blue, brown ok
    {{150, 150, 150}, {150, 0, 50}, {50, 150, 0}}           // level 9: white, rose, bright green
};

//const color_t color_lookup_table[COLOR_PALETTE_ROTATIONS][COLOR_PALETTES] = {
//    {{160, 160, 160}, {0, 160, 255}, {0, 64, 192}},        // level 0: soft white, medium blue, deep blue
//    {{160, 160, 160}, {128, 255, 0}, {32, 160, 32}},       // level 1: soft white, bright green, visible green (ok)
//    {{160, 160, 160}, {255, 64, 255}, {160, 0, 160}},      // level 2: soft white, vibrant magenta, bold magenta
//    {{160, 160, 160}, {128, 255, 128}, {48, 160, 48}},     // level 3: soft white, mint green, soft forest green
//    {{160, 160, 160}, {64, 255, 255}, {32, 160, 160}},     // level 4: soft white, bright cyan, medium cyan
//    {{160, 160, 160}, {128, 64, 255}, {0, 255, 160}},      // level 5: soft white, lavender, aqua green
//    {{160, 160, 160}, {96, 96, 96}, {255, 64, 0}},         // level 6: soft white, medium gray, vivid red-orange
//    {{160, 160, 160}, {200, 0, 50}, {128, 32, 255}},       // level 7: soft white, rich red, electric purple (ok)
//    {{160, 160, 160}, {255, 64, 64}, {64, 64, 192}},       // level 8: soft white, strong red, bright blue
//    {{160, 160, 160}, {255, 180, 64}, {220, 32, 32}}       // level 9: soft white, warm orange, strong red
//};


//@formatter:on
color_t get_color_palette(int current_level, int shape_id) {
    int rotation_index = current_level % COLOR_PALETTE_ROTATIONS;
    int shape_id_index = shape_id % COLOR_PALETTES;

    return color_lookup_table[rotation_index][shape_id_index];
}
