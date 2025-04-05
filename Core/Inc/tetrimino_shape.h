/*
 * tetrimino_shape.h
 *
 *  Created on: Jan 10, 2025
 *      Author: jdbnts
 */

#ifndef INC_TETRIMINO_SHAPE_H_
#define INC_TETRIMINO_SHAPE_H_

#include <stdint.h>
#include "tetrimino.h"

#define TETRIMINO_BLOCK_SIZE  (5)  // 5x5 block
#define TETRIMINO_CENTER_X    (2)
#define TETRIMINO_CENTER_Y    (2)
#define TETRIMINO_MASK        (0x1F)

extern const uint8_t tetrimino_shape[];
extern const uint8_t tetrimino_shape_offset_lut[TETRIMINO_COUNT][TETRIMINO_ROTATION_COUNT];
extern const uint8_t tetrimino_spawn[TETRIMINO_COUNT];
extern const uint8_t tetrimino_preview[TETRIMINO_COUNT];

#endif /* INC_TETRIMINO_SHAPE_H_ */
