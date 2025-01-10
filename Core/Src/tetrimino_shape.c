/*
 * tetrimino_shape.c
 *
 *  Created on: Jan 9, 2025
 *      Author: jdbnts
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
    0x00, 0x04, 0x0e, 0x00, 0x00, // T - up
    0x00, 0x04, 0x06, 0x04, 0x00, // T - right
    0x00, 0x00, 0x0e, 0x04, 0x00, // T - down (spawn)
    0x00, 0x04, 0x0c, 0x04, 0x00, // T - left
    0x00, 0x00, 0x00, 0x00, 0x00, // J - up
    0x00, 0x00, 0x00, 0x00, 0x00, // J - right
    0x00, 0x00, 0x00, 0x00, 0x00, // J - down (spawn)
    0x00, 0x00, 0x00, 0x00, 0x00, // J - left
    0x00, 0x00, 0x00, 0x00, 0x00, // Z - horizontal (spawn)
    0x00, 0x00, 0x00, 0x00, 0x00, // Z - vertical
    0x00, 0x00, 0x00, 0x00, 0x00, // O - spawn
    0x00, 0x00, 0x00, 0x00, 0x00, // S - horizontal (spawn)
    0x00, 0x00, 0x00, 0x00, 0x00, // S - vertical
    0x00, 0x00, 0x00, 0x00, 0x00, // L - up
    0x00, 0x00, 0x00, 0x00, 0x00, // L - right
    0x00, 0x00, 0x00, 0x00, 0x00, // L - down (spawn)
    0x00, 0x00, 0x00, 0x00, 0x00, // L - left
    0x00, 0x00, 0x00, 0x00, 0x00, // I - vertical
    0x00, 0x00, 0x00, 0x00, 0x00, // I - horizontal (spawn)
};

/**
 * Tetrimino shape offset lookup table for each rotation position (up, right, down, left, horizontal or vertical).
 *
 * For example, the T tetrimino has four rotations, so the offsets are 0, 5, 15, and 20.
 * The offsets are used to reference the tetrimino_shape array to get the shape for each rotation.
 *
 */

const uint8_t tetrimino_shape_offset_lut[TETRIMINO_COUNT][TETRIMINO_ROTATION_COUNT] = {
   { 0, 5, 10, 15 },    // T
   { 20, 25, 30, 35},   // J
   { 40, 45, 40, 45},   // Z
   { 50, 50, 50, 50},   // O
   { 55, 60, 55, 60},   // S
   { 65, 70, 75, 80},   // L
   { 85, 85, 85, 85}    // I
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


