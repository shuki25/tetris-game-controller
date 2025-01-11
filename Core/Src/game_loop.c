/**
 ******************************************************************************
 * @file           : game_loop.c
 * @author         : Patrick Lebeau Jr., Dr. Joshua Butler
 * @date           : Dec 9, 2024
 * @brief          : Main game loop for Classic Tetris on LED Grid
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

#include <stdint.h>

#include "renderer.h"
#include "main.h"

#include "snes_controller.h"
#include "game_loop.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "ws2812.h"
#include "matrix.h"
#include "itm_debug.h"
#include "util.h"
#include "cmsis_os.h"

// Debugger Expression Variables
uint32_t game_loop_counter = 0;
uint32_t render_count = 0;

// Matrix Variables
matrix_t matrix;

// Render Variables
uint8_t update_screen_flag;
led_t led;
uint8_t *brightness_lookup = NULL;
uint32_t render_delay = (1000000 / 10);
renderer_t renderer;
uint16_t lookup_table[MATRIX_HEIGHT][MATRIX_WIDTH];

// Controller Variables
snes_controller_t snes_controller;

// TIM Variables
extern TIM_HandleTypeDef htim3;

/**
 * @brief  Splash screen
 * @param  None
 * @retval None
 */
void splash() {
    update_screen_flag = 0;
    ssd1306_Init();
    ssd1306_Fill(Black);
    ssd1306_WriteString("RITetris", Font_16x26, White); // name undecided
    ssd1306_SetCursor(3, 28);
    ssd1306_UpdateScreen();
}

/**
 * @brief  Initialize game state
 * @param  None
 * @retval None
 */
void game_init() {
    splash();
    // TODO: Initialize game state (structs, bitboards, etc.)
}

/**
 * @brief  Main game loop for Classic Tetris on LED Grid
 * @param  None
 * @retval None
 */
void game_loop(void) {
    snes_controller_status_t controller_status;
    matrix_status_t matrix_status;
    renderer_status_t rendering_status;

    char output_buffer[80];

    // TODO: Load settings from EEPROM

    // TODO: Load high scores from EEPROM

    // TODO: Initialize game variables

    // TODO: Initialize game components
    /* Generate and initialize the brightness lookup table */

    controller_status = snes_controller_init(&snes_controller,
    SNES_LATCH_GPIO_Port, SNES_LATCH_Pin,
    SNES_CLOCK_GPIO_Port, SNES_CLOCK_Pin, SNES_DATA0_GPIO_Port,
    SNES_DATA0_Pin, 60);
    if (controller_status != SNES_CONTROLLER_OK) {
#if DEBUG_OUTPUT
        printf("SNES controller initialization failed\n");
#endif
    }

    // TODO: Start the main game loop
    brightness_lookup = generate_brightness_lookup_table(10);

    if (brightness_lookup != NULL) {
#if DEBUG_OUTPUT
        printf("Generated brightness lookup table\n");
#endif
    }

    matrix_status = matrix_init(&matrix);

    if (matrix_status == MATRIX_OK) {
#if DEBUG_OUTPUT
        printf("Matrix initialization success\n");
#endif
    }

    rendering_status = renderer_init(&renderer, lookup_table, &matrix, &led,
            &htim3, TIM_CHANNEL_1, render_delay);

    if (rendering_status == RENDERER_OK) {
#if DEBUG_OUTPUT
        printf("Rendering initialization success\n");
#endif
    }

    for (;;) {
        // TODO: Respond to scoreboard requests

        // TODO: Poll SNES controller
        controller_status = snes_controller_read(&snes_controller);
        if (controller_status == SNES_CONTROLLER_NO_STATE_CHANGE) {
            game_loop_counter++;
        }
        if (controller_status == SNES_CONTROLLER_STATE_CHANGE) {
#if DEBUG_OUTPUT
            snes_controller_print(&snes_controller);
#endif
        }

        // TODO: Check timer for game speed

        // TODO: Process input

        // TODO: Update tetromino rotation

        // TODO: Update tetromino position

        // TODO: Check for collision

        // TODO: Check for line clear

        // TODO: Check for topout condition

        // TODO: Is tetrimino locked in place?

        // TODO: Get the next tetrimino from the RNG

        // TODO: Render matrix and update LED grid

        rendering_status = renderer_test_render(&renderer);
#if DEBUG_OUTPUT
        if (rendering_status == RENDERER_UPDATED) {
            render_count++;
        }
#endif
        // TODO: Update UI
        game_loop_counter++;
        osThreadYield();
    }
}
