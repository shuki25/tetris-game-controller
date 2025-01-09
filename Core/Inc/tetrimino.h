/**
  ******************************************************************************
  * @file           : tetrimino.h
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

#ifndef INC_TETRIMINO_H_
#define INC_TETRIMINO_H_

// TODO: Typedef for tetrimino status in enum (e.g. TETRIMINO_OK, TETRIMINO_ERROR)
typedef enum {
    TETRIMINO_OK = 0, TETRIMINO_ERROR
} tetrimino_status_t;

// TODO: Constants for tetrimino pieces (e.g. TETRIMINO_I, TETRIMINO_J, TETRIMINO_L, TETRIMINO_O, TETRIMINO_S, TETRIMINO_T, TETRIMINO_Z
typedef enum {
    TETRIMINO_T = 0, TETRIMINO_J, TETRIMINO_Z, TETRIMINO_O, TETRIMINO_S, TETRIMINO_L, TETRIMINO_I, TETRIMINO_COUNT
} tetrimino_piece_t;

// TODO: Constants for tetrimino rotations (e.g. TETRIMINO_ROTATION_0, TETRIMINO_ROTATION_90, TETRIMINO_ROTATION_180, TETRIMINO_ROTATION_270)
typedef enum {
    TETRIMINO_ROTATION_0 = 0, TETRIMINO_ROTATION_90, TETRIMINO_ROTATION_180, TETRIMINO_ROTATION_270, TETRIMINO_ROTATION_COUNT
} tetrimino_rotation_t;

// TODO: Constants for rotation directions (e.g. ROTATE_LEFT, ROTATE_RIGHT)
typedef enum {
    ROTATE_LEFT = 0, ROTATE_RIGHT
} rotation_direction_t;

// TODO: Typedef for tetrimino struct (e.g. tetrimino_t)
typedef struct {
    tetrimino_piece_t piece;
    tetrimino_rotation_t rotation;
    uint8_t x;
    uint8_t y;
    tetrimino_piece_t next_piece;
} tetrimino_t;

// TODO: Function prototypes for tetrimino functions (e.g. tetrimino_init, tetrimino_rotate, tetrimino_move)

tetrmino_status_t tetrimino_init(tetrimino_t *tetrimino);
tetrmino_status_t tetrimino_rotate(tetrimino_t *tetrimino, rotation_direction_t direction);

#endif /* INC_TETRIMINO_H_ */
