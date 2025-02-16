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
#include <string.h>

#include "main.h"
#include "snes_controller.h"
#include "tetrimino.h"
#include "tetrimino_shape.h"
#include "game_loop.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "ws2812.h"
#include "renderer.h"
#include "matrix.h"
#include "itm_debug.h"
#include "util.h"
#include "cmsis_os.h"
#include "ring_buffer.h"
#include "ui.h"
#include "eeprom.h"

// Extern Variables
extern I2C_HandleTypeDef hi2c1;

// External variables

extern I2C_HandleTypeDef hi2c1;

// Debugger Expression Variables
uint32_t render_count = 0;
uint32_t controller_count = 0;
uint32_t redraw_screen_count = 0;
uint16_t controller_reading = 0;
uint32_t game_loop_count = 0;

// Matrix Variables
matrix_t matrix;

// Render Variables
uint8_t update_screen_flag;
led_t led;
uint8_t *brightness_lookup = NULL;
uint32_t render_delay = (1000000 / 33); // 30 FPS
renderer_t renderer;
uint16_t lookup_table[MATRIX_HEIGHT][MATRIX_WIDTH];

// Controller Variables
snes_controller_t snes_controller;
game_t game;
//tetrimino_t tetrimino;
//tetrimino_t tetrimino_pending;
game_high_score_t high_scores[EEPROM_NUM_HIGH_SCORES];
game_high_score_t *high_score_ptrs[EEPROM_NUM_HIGH_SCORES];
saved_settings_t settings;

// TIM Variables
extern TIM_HandleTypeDef htim3;

// EEPROM Variables
eeprom_t eeprom;
eeprom_id_t signature;

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
game_status_t game_init(void) {
//    splash();

    // TODO: Initialize game state (structs, bitboards, etc.)
    memset(&game, 0, sizeof(game_t));
    game.state = GAME_STATE_SPLASH;
    game.play_state = PLAY_STATE_NOT_STARTED;
    game.game_speed = 1000;
    game.drop_time_delay = 1000000;
    game.lock_time_delay = 500000; // 0.5 seconds
    game.line_clear_time_delay = 100000; // 0.1 seconds per LED pair

    return GAME_OK;
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
//    char output_buffer[80];
    tetrimino_t tetrimino;
    tetrimino_status_t tetrimino_status;
    uint8_t matrix_update_flag = 0;
    uint16_t controller_current_buttons;
    RingBuffer controller_buffer;
    uint32_t press_start_timer_start = 0;
    uint32_t press_start_state = 0;
    uint32_t fps_start_count = 0;
    uint32_t fps_end_count = 0;
    uint32_t fps_time_last_update = 0;
    uint32_t fps_time_diff = 0;
    uint32_t lines_to_be_cleared = 0;
    matrix_t temp_matrix;

    if (ring_buffer_init(&controller_buffer, 16, sizeof(uint16_t)) != RING_BUFFER_OK) {
#if DEBUG_OUTPUT
        printf("Failed to initialize ring buffer\n");
#endif
        Error_Handler();
    }

    // Init EEPROM
    eeprom_init(&eeprom, &hi2c1, EEPROM_GPIO_Port, EEPROM_Pin);

    // Verify EEPROM signature
    if (eeprom_get_signature(&eeprom, &signature) != EEPROM_OK) {
#if DEBUG_OUTPUT
        printf("Failed to retrieve EEPROM signature\n");
#endif
    }

    // Init arr pointers to high scores
    for (int i = 0; i < EEPROM_NUM_HIGH_SCORES; i++) {
        memset(&high_scores[i], 0, sizeof(game_high_score_t));
        high_score_ptrs[i] = &high_scores[i];
    }

    eeprom_status_t status = eeprom_verify_signature(&signature, EEPROM_NUM_USED_PAGES);
    if (status == EEPROM_SIGNATURE_MISMATCH) {
#if DEBUG_OUTPUT
        printf("EEPROM signature mismatch found\n");
#endif
        eeprom_generate_signature(&signature, EEPROM_NUM_USED_PAGES);
        eeprom_write_signature(&eeprom, &signature);

        // Initialize EEPROM with default values for settings and high scores
        eeprom_get_default_high_scores(high_score_ptrs);
        eeprom_get_default_settings(&settings);

        // Write default high scores and settings to EEPROM
        eeprom_write_settings(&eeprom, &settings);
        eeprom_write_high_scores(&eeprom, high_score_ptrs);
    } else {
#if DEBUG_OUTPUT
        printf("EEPROM signature matched\n");
#endif
        // Load settings from EEPROM
        memset(&settings, 0, sizeof(saved_settings_t));
        eeprom_get_settings(&eeprom, &settings);

        // Load high scores from EEPROM
        eeprom_get_high_scores(&eeprom, high_score_ptrs);
    }

    // TODO: Initialize game variables

    // TODO: Initialize game components

    // Initialize OLED display driver
    ui_init();

    controller_status = snes_controller_init(&snes_controller,
    SNES_LATCH_GPIO_Port, SNES_LATCH_Pin,
    SNES_CLOCK_GPIO_Port, SNES_CLOCK_Pin, SNES_DATA0_GPIO_Port,
    SNES_DATA0_Pin, 60);
    if (controller_status != SNES_CONTROLLER_OK) {
#if DEBUG_OUTPUT
        printf("SNES controller initialization failed\n");
#endif
    }
    tetrimino_status = tetrimino_init(&tetrimino);
#if DEBUG_OUTPUT
    if (tetrimino_status == TETRIMINO_OK) {
        tetrimino_debug_print(&tetrimino);
    }
#endif

    /* Generate and initialize the brightness lookup table */
    brightness_lookup = generate_brightness_lookup_table(15);

    if (brightness_lookup != NULL) {
#if DEBUG_OUTPUT
        printf("Generated brightness lookup table\n");
#endif
    }

    matrix_status = matrix_init(&matrix);

    if (matrix_status == MATRIX_OK) {
#if DEBUG_OUTPUT
        printf("Matrix initialization success\n");
        matrix_status = matrix_add_tetrimino(&matrix, &tetrimino);
        if (matrix_status == MATRIX_COLLISION_DETECTED) {
            printf("Add Tetrimino to Matrix: Collision detected\n");
        } else if (matrix_status == MATRIX_OUT_OF_BOUNDS) {
            printf("Add Tetrimino to Matrix: Out of bounds\n");
        }
        matrix_debug_print(&matrix);
#endif
    }

    rendering_status = renderer_init(&renderer, lookup_table, &matrix, &led, &htim3, TIM_CHANNEL_1,
            render_delay);

    if (rendering_status == RENDERER_OK) {
#if DEBUG_OUTPUT
        printf("Rendering initialization success\n");
#endif
    }

    rendering_status = renderer_create_boundary(&renderer);

    if (rendering_status == RENDERER_OK) {
#if DEBUG_OUTPUT
        printf("Rendering boundary success\n");
#endif
    }
    // If you want to test a feature, uncomment the following line
//    game.state = GAME_STATE_TEST_FEATURE;
//    game.state = GAME_STATE_GAME_IN_PROGRESS;

    // Test rendering, define tetrimino stack
    matrix.stack[0] = 0x15581FF0;
    matrix.stack[1] = 0x1FF81FD8;
    matrix.palette1[0] = 0x00101E10;
    matrix.palette2[0] = 0x004001E0;
    matrix.palette1[1] = 0x01800180;
    matrix.palette2[1] = 0x1C000800;

    for (;;) {
        // TODO: Respond to scoreboard requests

        // Poll SNES controller before any other processing in the state machine
        controller_status = snes_controller_read(&snes_controller);
        controller_reading = snes_controller.buttons_state;
        if (controller_status == SNES_CONTROLLER_DISCONNECTED && snes_controller.led_state == 1) {
            snes_controller.led_state = 0;
            HAL_GPIO_WritePin(LED_SNES0_GPIO_Port, LED_SNES0_Pin, GPIO_PIN_RESET);
#if DEBUG_OUTPUT
            printf("Controller disconnected\n");
#endif
        } else if (controller_status != SNES_CONTROLLER_DISCONNECTED
                && controller_status != SNES_CONTROLLER_NOT_READY && snes_controller.led_state == 0) {
            snes_controller.led_state = 1;
            HAL_GPIO_WritePin(LED_SNES0_GPIO_Port, LED_SNES0_Pin, GPIO_PIN_SET);
        }
        if (controller_status == SNES_CONTROLLER_STATE_CHANGE) {
            if (ring_buffer_enqueue(&controller_buffer, &snes_controller.buttons_state) == false) {
#if DEBUG_OUTPUT
                printf("Controller buffer is full. Dropping.\n");
#endif
                Error_Handler();
            }
            if (snes_controller.buttons_state) {
                controller_count++;
            }
        }

        switch (game.state) {

        /* ---------------------- SPLASH SCREEN ---------------------- */
        case GAME_STATE_SPLASH:
            ui_splash_screen();
            game.state = GAME_STATE_SPLASH_WAIT;

            break;

            /* ------------------------- SPLASH WAIT ------------------------ */
        case GAME_STATE_SPLASH_WAIT:
            if (ring_buffer_dequeue(&controller_buffer, &controller_current_buttons) == true) {
                if (controller_current_buttons & SNES_BUTTON_START) {
//                    game.state = GAME_STATE_MENU;
                    game.state = GAME_STATE_PREPARE_GAME;
                    ssd1306_Fill(Black);
                    ssd1306_UpdateScreen();
                    break;
                }
            }
            if (util_time_expired_delay(press_start_timer_start, 500000)) {  // 500ms
                press_start_timer_start = TIM2->CNT;
                press_start_state = !press_start_state;
                ssd1306_SetCursor(30, 55);
                if (press_start_state) {
                    ssd1306_WriteString("                ", Font_6x8, White); // erase line
                } else {
                    ssd1306_WriteString("Press start", Font_6x8, White);
                }
                ssd1306_UpdateScreen();
                redraw_screen_count++;
            }
            break;

            /* ------------------------- MAIN MENU -------------------------- */
        case GAME_STATE_MENU:
            // TODO: Display main menu
            break;

            /* ------------------------ PLAYING MENU ------------------------ */
        case GAME_STATE_PLAY_MENU:
            // TODO: Display playing menu
            break;

            /* -------------------- PREPARE GAME STATE ---------------------- */
        case GAME_STATE_PREPARE_GAME:
            // TODO: Initialize game variables
            game.score = 0;
            game.level = 1;
            game.lines = 0;
            game.drop_time_delay = 700000;
            game.drop_time_start = TIM2->CNT;

            game.state = GAME_STATE_GAME_IN_PROGRESS;
            game.play_state = PLAY_STATE_NORMAL;
            break;

            /* ---------------------- GAME IN PROGRESS ---------------------- */
        case GAME_STATE_GAME_IN_PROGRESS:
            if (ring_buffer_dequeue(&controller_buffer, &controller_current_buttons) == true) {
                controller_status = SNES_CONTROLLER_STATE_CHANGE;
            } else {
                controller_status = SNES_CONTROLLER_NO_STATE_CHANGE;
            }

            if (controller_status == SNES_CONTROLLER_STATE_CHANGE) {
                matrix_update_flag = 0;
                if (controller_current_buttons & SNES_BUTTON_A) {
                    tetrimino_status = tetrimino_rotate(&tetrimino, ROTATE_CW);
                } else if (controller_current_buttons & SNES_BUTTON_B) {
                    tetrimino_status = tetrimino_rotate(&tetrimino, ROTATE_CCW);
                } else if (controller_current_buttons & SNES_BUTTON_R) {
                    tetrimino.piece++;
                    if (tetrimino.piece >= TETRIMINO_COUNT) {
                        tetrimino.piece = 0;
                    }
                    tetrimino.shape_offset = tetrimino_shape_offset_lut[tetrimino.piece][tetrimino.rotation];
                    tetrimino_status = TETRIMINO_REFRESH;

                } else if (controller_current_buttons & SNES_BUTTON_L) {
                    tetrimino.piece--;
                    if (tetrimino.piece >= TETRIMINO_COUNT) {
                        tetrimino.piece = TETRIMINO_COUNT - 1;
                    }
                    tetrimino.shape_offset = tetrimino_shape_offset_lut[tetrimino.piece][tetrimino.rotation];
                    tetrimino_status = TETRIMINO_REFRESH;
                } else if (controller_current_buttons & SNES_BUTTON_UP) {
                    tetrimino.y++;
                    if (tetrimino.y >= PLAYING_FIELD_HEIGHT + TETRIMINO_CENTER_Y) {
                        tetrimino.y = PLAYING_FIELD_HEIGHT + TETRIMINO_CENTER_Y - 1;
                    }
                    matrix_status = matrix_add_tetrimino(&matrix, &tetrimino);
                    if (matrix_status == MATRIX_COLLISION_DETECTED) {
                        tetrimino.y--;
                    } else if (matrix_status == MATRIX_OUT_OF_BOUNDS) {
                        tetrimino.y--;
                    }
                    matrix_update_flag = 1;
                } else if (controller_current_buttons & SNES_BUTTON_DOWN) {
                    if (tetrimino.y > 0) {
                        tetrimino.y--;
                    }
                    matrix_status = matrix_add_tetrimino(&matrix, &tetrimino);
                    if (matrix_status == MATRIX_COLLISION_DETECTED) {
                        tetrimino.y++;
                    } else if (matrix_status == MATRIX_OUT_OF_BOUNDS) {
                        tetrimino.y++;
                    }
                    matrix_update_flag = 1;
                } else if (controller_current_buttons & SNES_BUTTON_LEFT) {
                    tetrimino.x--;
                    if (tetrimino.x > PLAYING_FIELD_WIDTH) {
                        tetrimino.x = 0;
                    }
                    matrix_status = matrix_add_tetrimino(&matrix, &tetrimino);
                    if (matrix_status == MATRIX_COLLISION_DETECTED) {
                        tetrimino.x++;
                    } else if (matrix_status == MATRIX_OUT_OF_BOUNDS) {
                        tetrimino.x++;
                    }
                    matrix_update_flag = 1;
                } else if (controller_current_buttons & SNES_BUTTON_RIGHT) {
                    tetrimino.x++;
                    if (tetrimino.x >= PLAYING_FIELD_WIDTH) {
                        tetrimino.x = PLAYING_FIELD_WIDTH - 1;
                    }
                    matrix_status = matrix_add_tetrimino(&matrix, &tetrimino);
                    if (matrix_status == MATRIX_COLLISION_DETECTED) {
                        tetrimino.x--;
                    } else if (matrix_status == MATRIX_OUT_OF_BOUNDS) {
                        tetrimino.x--;
                    }
                    matrix_update_flag = 1;
                } else if (controller_current_buttons & SNES_BUTTON_Y) {
                    game.drop_time_delay += 25000;
                } else if (controller_current_buttons & SNES_BUTTON_X) {
                    game.drop_time_delay -= 25000;
                    if (game.drop_time_delay < 25000) {
                        game.drop_time_delay = 25000;
                    }
                } else {
                    tetrimino_status = TETRIMINO_OK;
                }
                if (matrix_status != MATRIX_REFRESH && matrix_update_flag) {
                    // Revert tetrimino position and refresh matrix
                    matrix_status = matrix_add_tetrimino(&matrix, &tetrimino);
                } else if (tetrimino_status == TETRIMINO_REFRESH) {
                    matrix_status = matrix_add_tetrimino(&matrix, &tetrimino);
                    if (matrix_status == MATRIX_COLLISION_DETECTED) {
                        // TODO: Reverse tetrimino position
                    }
                    matrix_update_flag = 1;
                }
#if DEBUG_OUTPUT
                snes_controller.buttons_state = controller_current_buttons;
                snes_controller_print(&snes_controller);
                if (tetrimino_status == TETRIMINO_REFRESH && snes_controller.buttons_state) {
                    tetrimino_debug_print(&tetrimino);
                }
                if (matrix_status == MATRIX_COLLISION_DETECTED) {
                    printf("Matrix collision detected\n");
                } else if (matrix_status == MATRIX_OUT_OF_BOUNDS) {
                    printf("Matrix out of bounds\n");
                }
                if (matrix_update_flag) {
                    matrix_debug_print(&matrix);
                    matrix_status = MATRIX_NO_CHANGE;
                } else {
                    printf("Matrix status: %d\n", matrix_status);
                }
#endif
            } // end if controller_status == SNES_CONTROLLER_STATE_CHANGE

            // TODO: Check timer for game speed
            if (game.play_state == PLAY_STATE_NORMAL) {
                matrix_copy(&temp_matrix, &matrix); // Save current matrix state
                if (util_time_expired_delay(game.drop_time_start, game.drop_time_delay)) {
                    if (tetrimino.y > 0) {
                        tetrimino.y--;
                    }

                    matrix_status = matrix_add_tetrimino(&matrix, &tetrimino);
                    if (matrix_status == MATRIX_COLLISION_DETECTED) {
                        tetrimino.y++;
                    } else if (matrix_status == MATRIX_OUT_OF_BOUNDS) {
                        tetrimino.y++;
                    } else if (matrix_status == MATRIX_REACHED_BOTTOM) {
                        tetrimino.y++;
                        matrix_status = matrix_add_tetrimino(&matrix, &tetrimino);
                        game.play_state = PLAY_STATE_HALF_SECOND_B4_LOCK;
                        game.lock_time_start = TIM2->CNT;
                    } else {
                        // Check for collision with the current stack
                        matrix_status = matrix_check_collision(&matrix, &tetrimino);
                        if (matrix_status == MATRIX_STACK_COLLISION) {
                            // Restore previous matrix state
                            matrix_copy(&matrix, &temp_matrix);
                            game.play_state = PLAY_STATE_HALF_SECOND_B4_LOCK;
                            game.lock_time_start = TIM2->CNT;
                            //                    tetrimino_status = tetrimino_next(&tetrimino);
                            //                    matrix_status = matrix_add_tetrimino(&matrix,&tetrimino);

                        }
                        if (tetrimino.y == 0) { // Long bar reached to bottom of matrix, transition to lock state
                            game.play_state = PLAY_STATE_HALF_SECOND_B4_LOCK;
                            game.lock_time_start = TIM2->CNT;
                        }
                        game.drop_time_start = TIM2->CNT;
                    }
                    matrix_update_flag = 1;
                }
            } else {
                if (game.play_state == PLAY_STATE_HALF_SECOND_B4_LOCK) {
                    if (util_time_expired_delay(game.lock_time_start, game.lock_time_delay)) {
                        game.play_state = PLAY_STATE_LOCKED;
                    }
                }
            }

            // TODO: Process input

            // TODO: Update tetromino rotation

            // TODO: Update tetromino position

            // TODO: Check for collision

            // TODO: Check for line clear

            // TODO: Check for topout condition

            // TODO: Is tetrimino locked in place?

            // TODO: Get the next tetrimino from the RNG
            if (game.play_state == PLAY_STATE_LOCKED) {
                // TODO: Merge playfield with stack & palette

                // Check for line clear
                lines_to_be_cleared = matrix_check_line_clear(&matrix);
#if DEBUG_OUTPUT
                if (lines_to_be_cleared) {
                    printf("Lines to be cleared: ");
                    for (int i = 0; i < PLAYING_FIELD_HEIGHT; i++) {
                        if (lines_to_be_cleared & (1 << i)) {
                            printf("%d ", i);
                        }
                    }
                    printf("\n");
                }
#endif
                if (lines_to_be_cleared) {
                    game.play_state = PLAY_STATE_LINE_CLEAR;
                    game.line_clear_time_start = TIM2->CNT;
                } else {
                    game.play_state = PLAY_STATE_NEXT_TETRIMINO;
                }
            }

            // TODO: Process line clear if needed
            if (game.play_state == PLAY_STATE_LINE_CLEAR) {
                if (util_time_expired_delay(game.line_clear_time_start, game.line_clear_time_delay)) {
                    if (matrix_line_clear(&matrix, lines_to_be_cleared)) {  // Is line clear complete?
                        game.play_state = PLAY_STATE_NEXT_TETRIMINO;  // Move to next tetrimino
                        game.drop_time_start = TIM2->CNT;
                        lines_to_be_cleared = 0;
                    }
                }
            }

            // TODO: Process input

            // TODO: Update tetromino rotation

            // TODO: Update tetromino position

            // TODO: Check for collision

            // TODO: Check for line clear

            // TODO: Check for topout condition

            // TODO: Is tetrimino locked in place?

            // TODO: Get the next tetrimino from the RNG
            if (game.play_state == PLAY_STATE_NEXT_TETRIMINO) {
                tetrimino_status = tetrimino_next(&tetrimino);
                if (tetrimino_status == TETRIMINO_OK) {
                    matrix_status = matrix_add_tetrimino(&matrix, &tetrimino);
                    if (matrix_status == MATRIX_COLLISION_DETECTED) { // TOPPED OUT
                        game.state = GAME_STATE_GAME_ENDED;
                        game.play_state = PLAY_STATE_TOP_OUT;
                    } else {
                        game.play_state = PLAY_STATE_NORMAL;
                        game.drop_time_start = TIM2->CNT;
                    }
                }
            }

            // TODO: Render matrix and update LED grid
            rendering_status = renderer_render(&renderer, &matrix);
            if (rendering_status == RENDERER_UPDATED) {
                render_count++;
            }
            // TODO: Update UI
            fps_time_diff = util_time_diff_us(fps_time_last_update, TIM2->CNT);
            if (fps_time_diff >= 500000) {
                fps_start_count = fps_end_count;
                fps_end_count = render_count;
                fps_time_last_update = TIM2->CNT;
                ui_display_fps(fps_start_count, fps_end_count, fps_time_diff);
            }
            break;

            /* ------------------------- PAUSE MENU ------------------------ */
        case GAME_STATE_PAUSE:
            // TODO: Display pause menu
            break;

            /* -------------------------- GAME OVER ------------------------ */
        case GAME_STATE_GAME_ENDED:
            // TODO: Display game over screen

            // Persist settings and high scores by writing them to EEPROM
            eeprom_write_settings(&eeprom, &settings);
            eeprom_write_high_scores(&eeprom, high_score_ptrs);

            break;

            /* ------------------------ HIGH SCORES ------------------------ */
        case GAME_STATE_HIGH_SCORE:
            // TODO: Display high scores
            break;

            /* ------------------------ SETTINGS MENU ---------------------- */
        case GAME_STATE_SETTINGS:
            // TODO: Display settings menu
            break;

            /* ------------------------ TEST FEATURE ------------------------ */
        case GAME_STATE_TEST_FEATURE:
            /* Developer test code START */

//            rendering_status = renderer_test_render(&renderer);
//#if DEBUG_OUTPUT
//            if (rendering_status == RENDERER_UPDATED) {
//                render_count++;
//            }
//#endif
            /* Developer test code END */
            break;

            /* ----------------------- UNKNOWN STATES ---------------------- */
        default:
            // TODO: Handle unknown states (fail-safe)
            break;
        } // end switch

        game_loop_count++;
        osThreadYield();

    } // end for loop
} // end game_loop
