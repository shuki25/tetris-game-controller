/**
 ******************************************************************************
 * @file           : game_loop.c
 * @author         : Patrick Lebeau Jr., Dr. Joshua Butler
 * @date           : Jan 9, 2024
 * @brief          : Tetrimino shape definitions
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

#include "tetrimino_shape.h"

// @formatter:off

/**
 * Tetrimino shapes
 *
 * Each tetrimino is represented by a 5x5 block of bits with the center of rotation at (2, 2).
 * The block is stored in a five 8-bit integers where the LSB is on the right.
 *
 * The tetrimino shapes were built using the online led matrix font generator at:
 * https://www.riyas.org/2013/12/online-led-matrix-font-generator-with.html
 *
 * Although the online generator generates 8x8 blocks, the tetrimino shapes were
 * created using 5x5 blocks, so the last three integers were truncated.
 *
 */

const uint8_t tetrimino_shape[] = {
    0x00, 0x04, 0x0e, 0x00, 0x00, // T - up [offset 0]
    0x00, 0x04, 0x06, 0x04, 0x00, // T - right [offset 5]
    0x00, 0x00, 0x0e, 0x04, 0x00, // T - down (spawn) [offset 10]
    0x00, 0x04, 0x0c, 0x04, 0x00, // T - left [offset 15]
    0x00, 0x08, 0x0e, 0x00, 0x00, // J - up [offset 20]
    0x00, 0x06, 0x04, 0x04, 0x00, // J - right [offset 25]
    0x00, 0x00, 0x0e, 0x02, 0x00, // J - down (spawn) [offset 30]
    0x00, 0x04, 0x04, 0x0c, 0x00, // J - left [offset 35]
    0x00, 0x00, 0x0c, 0x06, 0x00, // Z - horizontal (spawn) [offset 40]
    0x00, 0x02, 0x06, 0x04, 0x00, // Z - vertical [offset 45]
    0x00, 0x00, 0x0c, 0x0c, 0x00, // O - spawn [offset 50]
    0x00, 0x00, 0x06, 0x0c, 0x00, // S - horizontal (spawn) [offset 55]
    0x00, 0x04, 0x06, 0x02, 0x00, // S - vertical [offset 60]
    0x00, 0x02, 0x0e, 0x00, 0x00, // L - up [offset 65]
    0x00, 0x04, 0x04, 0x06, 0x00, // L - right [offset 70]
    0x00, 0x00, 0x0e, 0x08, 0x00, // L - down (spawn) [offset 75]
    0x00, 0x0c, 0x04, 0x04, 0x00, // L - left [offset 80]
    0x04, 0x04, 0x04, 0x04, 0x00, // I - vertical [offset 85]
    0x00, 0x00, 0x1e, 0x00, 0x00, // I - horizontal (spawn) [offset 90]
};

/**
 * Tetrimino shape offset lookup table for each rotation position (up, right, down, left, horizontal or vertical).
 *
 * For example, the T tetrimino has four rotations, so the offsets are 0, 5, 15, and 20.
 * The offsets are used to reference the tetrimino_shape array to get the shape for each rotation.
 *
 * Offset mapping is ordered by tetrimino piece (T, J, Z, O, S, L, I) and then by rotation position (up, right, down, left).
 *
 */

const uint8_t tetrimino_shape_offset_lut[TETRIMINO_COUNT][TETRIMINO_ROTATION_COUNT] = {
   { 0, 5, 10, 15 },    // T
   { 20, 25, 30, 35},   // J
   { 40, 45, 40, 45},   // Z
   { 50, 50, 50, 50},   // O
   { 55, 60, 55, 60},   // S
   { 65, 70, 75, 80},   // L
   { 85, 90, 85, 90}    // I
};

/**
 * Tetrimino spawn rotation position lookup table for each tetrimino piece (T, J, Z, O, S, L, I).
 *
 * The spawn lookup table is the spawned rotation position for each tetrimino piece which is referenced
 * in the tetrimino_shape_offset_lut table.
 *
 */

const uint8_t tetrimino_spawn[TETRIMINO_COUNT] = { 2, 2, 0, 0, 0, 2, 1 };

// @formatter:on
