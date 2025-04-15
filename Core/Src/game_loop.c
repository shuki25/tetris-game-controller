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
#include "tetris.h"
#include "game_loop.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "ws2812.h"
#include "renderer.h"
#include "matrix.h"
#include "itm_debug.h"
#include "util.h"
#include "ring_buffer.h"
#include "ui.h"
#include "eeprom.h"
#include "led_indicator.h"

// Extern Variables
extern TIM_HandleTypeDef htim2;
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
uint32_t render_delay = (1000000 / 35); // 30 FPS
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

// UI variables
ui_menu_t menu;
ui_state_t ui_level_selection_mode = UI_LEVEL_SELECTION_DRAW;
uint8_t ui_is_cursor_on = 0;
uint32_t ui_cursor_start_time = 0;

// EEPROM Variables
eeprom_t eeprom;
eeprom_id_t signature;

// LED Indicators
led_indicator_t hb_led;
led_indicator_t rj45_led;

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
    // Initialize game to zero
    memset(&game, 0, sizeof(game_t));

    // Set game states to default values
    game.state = GAME_STATE_SPLASH;
    game.play_state = PLAY_STATE_NOT_STARTED;
    game.drop_time_delay = 1000000;
    game.lock_time_delay = 500000; // 0.5 seconds

    return GAME_OK;
}

/**
 * @brief  Main game loop for Classic Tetris on LED Grid
 * @param  None
 * @retval None
 */
void game_loop(void) {
    snes_controller_status_t controller_status;
    snes_controller_das_t controller_repeat_left;
    snes_controller_das_t controller_repeat_right;
    snes_controller_das_t controller_repeat_up;
    snes_controller_das_t controller_repeat_down;
    matrix_status_t matrix_status;
    renderer_status_t rendering_status;
//    char output_buffer[80];
    tetrimino_t tetrimino;
    tetrimino_status_t tetrimino_status;
    uint16_t controller_current_buttons;
    uint16_t controller_previous_buttons = 0;
    RingBuffer controller_buffer;
    uint32_t press_start_timer_start = 0;
    uint32_t press_start_state = 0;
    uint32_t fps_start_count = 0;
    uint32_t fps_end_count = 0;
    uint32_t fps_time_last_update = 0;
    uint32_t fps_time_diff = 0;
    uint32_t lines_to_be_cleared = 0;
    matrix_t temp_matrix;
    matrix_t rotate_check_matrix;
    tetrimino_t temp_tetrimino;
    tetris_statistics_t tetris_statistics;

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

    eeprom_status_t status = eeprom_verify_signature(&signature,
    EEPROM_NUM_USED_PAGES);
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

    // Initialize OLED display driver
    memset(&tetris_statistics, 0, sizeof(tetris_statistics_t));
    ui_init(&tetris_statistics);

    controller_status = snes_controller_init(&snes_controller,
    SNES_LATCH_GPIO_Port, SNES_LATCH_Pin,
    SNES_CLOCK_GPIO_Port, SNES_CLOCK_Pin, SNES_DATA0_GPIO_Port,
    SNES_DATA0_Pin, 60);
    if (controller_status != SNES_CONTROLLER_OK) {
#if DEBUG_OUTPUT
        printf("SNES controller initialization failed\n");
#endif
    }
    if (snes_controller_delayed_auto_shift_init(&controller_repeat_left, SNES_BUTTON_LEFT)
            != SNES_CONTROLLER_DAS_OK) {
#if DEBUG_OUTPUT
        printf("SNES controller left repeater initialization failed\n");
#endif
    }

    if (snes_controller_delayed_auto_shift_init(&controller_repeat_right, SNES_BUTTON_RIGHT)
            != SNES_CONTROLLER_DAS_OK) {
#if DEBUG_OUTPUT
        printf("SNES controller right repeater initialization failed\n");
#endif
    }

    if (snes_controller_delayed_auto_shift_init(&controller_repeat_up, SNES_BUTTON_UP)
            != SNES_CONTROLLER_DAS_OK) {
#if DEBUG_OUTPUT
        printf("SNES controller up repeater initialization failed\n");
#endif
    }

    if (snes_controller_delayed_auto_shift_init(&controller_repeat_down, SNES_BUTTON_DOWN)
            != SNES_CONTROLLER_DAS_OK) {
#if DEBUG_OUTPUT
        printf("SNES controller down repeater initialization failed\n");
#endif
    }

    tetrimino_status = tetrimino_init(&tetrimino);
#if DEBUG_OUTPUT
    if (tetrimino_status == TETRIMINO_OK) {
        tetrimino_debug_print(&tetrimino);
    }
#endif

    /* Generate and initialize the brightness lookup table */
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
//        matrix_status = matrix_add_tetrimino(&matrix, &tetrimino);
//        if (matrix_status == MATRIX_COLLISION_DETECTED) {
//            printf("Add Tetrimino to Matrix: Collision detected\n");
//        } else if (matrix_status == MATRIX_OUT_OF_BOUNDS) {
//            printf("Add Tetrimino to Matrix: Out of bounds\n");
//        }
//        matrix_debug_print(&matrix);
#endif
    }

    rendering_status = renderer_init(&renderer, lookup_table, &matrix, &led, &htim3, TIM_CHANNEL_1,
            render_delay);

    if (rendering_status == RENDERER_OK) {
#if DEBUG_OUTPUT
        printf("Rendering initialization success\n");
#endif
    }

    // Initialize menu system
    ui_menu_init(&menu);

    rendering_status = renderer_create_boundary(&renderer);

    if (rendering_status == RENDERER_OK) {
#if DEBUG_OUTPUT
        printf("Rendering boundary success\n");
#endif
    }

    // Initialize LED Indicators
    led_init(&hb_led, LED_HB_GPIO_Port, LED_HB_Pin, &htim2);
    led_init(&rj45_led, I2C_LED_GPIO_Port, I2C_LED_Pin, &htim2);
    led_set_mode(&hb_led, LED_BLINK_CONTINUOUS);
    led_set_blink_delay(&hb_led, 500, 500);
    led_set_mode(&rj45_led, LED_N_BLINK);
    led_set_blink_delay(&rj45_led, 20, 20);
    led_set_n_blinks(&rj45_led, 10);
    hb_led.uses_pull_up = 0;
    rj45_led.uses_pull_up = 1;
    hb_led.active = 1;
    rj45_led.active = 1;

//     If you want to test a feature, uncomment the following line
//    game.state = GAME_STATE_TEST_FEATURE;
//    game.state = GAME_STATE_GAME_IN_PROGRESS;

    // Test rendering, define tetrimino stack
//    matrix.stack[0] = 0xff80ff8;
//    matrix.stack[1] = 0xff80ff8;
//    matrix.stack[2] = 0xff80ff8;
//    matrix.stack[3] = 0xff80ff8;
//    matrix.stack[4] = 0xff80ff8;
//    matrix.stack[5] = 0xff80fb8;
//    matrix.stack[6] = 0xff80df8;
//    matrix.stack[7] = 0x0;
//    matrix.stack[8] = 0;
//    matrix.stack[9] = 0x0;
//    matrix.palette1[0] = 0;
//    matrix.palette1[1] = 23069152;
//    matrix.palette1[2] = 33554496;
//    matrix.palette1[3] = 251661824;
//    matrix.palette1[4] = 33557248;
//    matrix.palette1[5] = 100663808;
//    matrix.palette1[6] = 1024;
//    matrix.palette1[7] = 0;
//    matrix.palette1[8] = 0;
//    matrix.palette1[9] = 0;
//    matrix.palette2[0] = 201328160;
//    matrix.palette2[1] = 234884096;
//    matrix.palette2[2] = 16777728;
//    matrix.palette2[3] = 12583360;
//    matrix.palette2[4] = 6291696;
//    matrix.palette2[5] = 25165824;
//    matrix.palette2[6] = 31457664;
//    matrix.palette2[7] = 0;
//    matrix.palette2[8] = 0;
//    matrix.palette2[9] = 0;
    ui_reset_ui_stats();

    for (;;) {
        // Future: Respond to scoreboard requests

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

            }
            if (snes_controller.buttons_state) {
                controller_count++;
            }
        }

        // Check for Delayed Auto Shift (DAS) events and enqueue them at predefined intervals

        snes_controller_delayed_auto_shift(&controller_repeat_left, &snes_controller);
        if (controller_repeat_left.repeat_status == SNES_CONTROLLER_DAS_ACTIVE_ENQUEUE) {
            if (ring_buffer_enqueue(&controller_buffer, &controller_repeat_left.target_button) == false) {
#if DEBUG_OUTPUT
                printf("Controller buffer is full. Dropping.\n");
#endif
            }
        }
        snes_controller_delayed_auto_shift(&controller_repeat_right, &snes_controller);
        if (controller_repeat_right.repeat_status == SNES_CONTROLLER_DAS_ACTIVE_ENQUEUE) {
            if (ring_buffer_enqueue(&controller_buffer, &controller_repeat_right.target_button) == false) {
#if DEBUG_OUTPUT
                printf("Controller buffer is full. Dropping.\n");
#endif
            }
        }

        if (game.state == GAME_STATE_PLAY_MENU) {
            snes_controller_delayed_auto_shift(&controller_repeat_up, &snes_controller);
            if (controller_repeat_up.repeat_status == SNES_CONTROLLER_DAS_ACTIVE_ENQUEUE) {
                if (ring_buffer_enqueue(&controller_buffer, &controller_repeat_up.target_button) == false) {
#if DEBUG_OUTPUT
                    printf("Controller buffer is full. Dropping.\n");
#endif
                }
            }

            snes_controller_delayed_auto_shift(&controller_repeat_down, &snes_controller);
            if (controller_repeat_down.repeat_status == SNES_CONTROLLER_DAS_ACTIVE_ENQUEUE) {
                if (ring_buffer_enqueue(&controller_buffer, &controller_repeat_down.target_button) == false) {
#if DEBUG_OUTPUT
                    printf("Controller buffer is full. Dropping.\n");
#endif
                }
            }
        }
        //game.state = GAME_STATE_TEST_FEATURE;

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
                    game.state = GAME_STATE_MENU;
                    ssd1306_Fill(Black);
                    ssd1306_UpdateScreen();
                    break;
                }
            }
            if (util_time_expired_delay(press_start_timer_start, 500000)) { // 500ms
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
            ui_menu_id_set(&menu, 0);
            ui_main_menu_selection(&menu);
            if (util_time_expired_delay(menu.cursor_start_time, 500000)) {
                menu.cursor_start_time = TIM2->CNT;
                ui_menu_cursor_blink(&menu);
            }
            if (ring_buffer_dequeue(&controller_buffer, &controller_current_buttons) == true) {
                if (controller_current_buttons & SNES_BUTTON_UP) {
                    ui_menu_controller_move_up(&menu);
                }
                if (controller_current_buttons & SNES_BUTTON_DOWN) {
                    ui_menu_controller_move_down(&menu);
                }

                if (controller_current_buttons & SNES_BUTTON_A) {
                    switch (menu.current_selection_id) {
                    case 0:
                        game.state = GAME_STATE_PLAY_MENU;
                        ssd1306_Fill(Black);
                        break;
                    case 1:
                        game.state = GAME_STATE_HIGH_SCORE;
                        ui_reset_ui_stats(); // Needed to initialize values for switching frames
                        ssd1306_Fill(Black);
                        break;
                    case 2:
                        game.state = GAME_STATE_PREPARE_GAME;
//                            game.state = GAME_STATE_SETTINGS;
                        ssd1306_Fill(Black);
                        break;
                    case 3:
                        game.state = GAME_STATE_PREPARE_GAME;
//                            game.state = GAME_STATE_CREDITS;
                        ssd1306_Fill(Black);
                        break;
                    }
                }
            }
            break;

            /* ------------------------ PLAYING MENU ------------------------ */
        case GAME_STATE_PLAY_MENU:
            if (util_time_expired_delay(menu.cursor_start_time, 500000)) {
                menu.cursor_start_time = TIM2->CNT;
                ui_level_selection_mode = UI_LEVEL_SELECTION_DRAW;
                ui_level_selection(&game.level, &ui_level_selection_mode, &ui_is_cursor_on);
            }
            ui_level_selection(&game.level, &ui_level_selection_mode, &ui_is_cursor_on);
            if (ring_buffer_dequeue(&controller_buffer, &controller_current_buttons) == true) {
                if (controller_current_buttons & SNES_BUTTON_DOWN) {
                    if (game.level == 0) {
                        game.level = 255;
                    } else {
                        game.level--;
                    }
                    ui_level_selection_mode = UI_LEVEL_SELECTION_DRAW;
                }
                if (controller_current_buttons & SNES_BUTTON_UP) {
                    if (game.level == 255) {
                        game.level = 0;
                    } else {
                        game.level++;
                    }
                    ui_level_selection_mode = UI_LEVEL_SELECTION_DRAW;
                }

                if (controller_current_buttons & SNES_BUTTON_START) {
                    game.state = GAME_STATE_PREPARE_GAME;
                    ssd1306_Fill(Black);
                }

                if (controller_current_buttons & SNES_BUTTON_B) {
                    menu.ui_status = UI_MENU_DRAW;
                    game.state = GAME_STATE_MENU;
                    ssd1306_Fill(Black);
                }
            }
            break;

            /* -------------------- PREPARE GAME STATE ---------------------- */
        case GAME_STATE_PREPARE_GAME:
            // Initialize game variables

            matrix_clear(&matrix);
            game.score = 0;
//            game.level = 0;
            game.lines = 0;
            game.lines_to_next_level = 10 * (game.level + 1);
            game.drop_time_normal_delay = tetrimino_drop_speed(game.level);
            game.drop_time_soft_drop_delay = game.drop_time_normal_delay / 20;
            game.drop_time_delay = game.drop_time_normal_delay;
            game.drop_time_start = TIM2->CNT;

            game.state = GAME_STATE_GAME_IN_PROGRESS;
            game.play_state = PLAY_STATE_NORMAL;

            memset(&game.stats, 0, sizeof(game_stats_t));

            // Reinitialize tetrimino piece
            tetrimino_init(&tetrimino);

            // reset game statistics and timer
            tetris_statistics_reset(&tetris_statistics);
            ui_reset_ui_stats();
            tetris_statistics.tetriminos_frequency[tetrimino.piece]++;
            break;

            /* ---------------------- GAME IN PROGRESS ---------------------- */
        case GAME_STATE_GAME_IN_PROGRESS:
            if (ring_buffer_dequeue(&controller_buffer, &controller_current_buttons) == true) {
                controller_status = SNES_CONTROLLER_STATE_CHANGE;
            } else {
                controller_status = SNES_CONTROLLER_NO_STATE_CHANGE;
            }

            // Determine if the controller button is in repeat mode (if the button is held down)

            if (controller_status == SNES_CONTROLLER_STATE_CHANGE) {
                tetrimino_copy(&temp_tetrimino, &tetrimino);
                matrix_copy(&temp_matrix, &matrix);
                matrix_copy(&rotate_check_matrix, &matrix);
                if (controller_current_buttons & SNES_BUTTON_A
                        && !(controller_previous_buttons & SNES_BUTTON_A)) {
                    tetrimino_status = tetrimino_rotate(&tetrimino, ROTATE_CW);
                    matrix_status = matrix_add_tetrimino(&rotate_check_matrix, &tetrimino);
                    if (matrix_check_collision(&rotate_check_matrix, &tetrimino) == MATRIX_STACK_COLLISION) {
                        tetrimino_copy(&tetrimino, &temp_tetrimino); // Revert tetrimino position
                        tetrimino_status = TETRIMINO_REFRESH;
                    }
                } else if (controller_current_buttons & SNES_BUTTON_B
                        && !(controller_previous_buttons & SNES_BUTTON_B)) {
                    tetrimino_status = tetrimino_rotate(&tetrimino, ROTATE_CCW);
                    matrix_status = matrix_add_tetrimino(&rotate_check_matrix, &tetrimino);
                    if (matrix_check_collision(&rotate_check_matrix, &tetrimino) == MATRIX_STACK_COLLISION) {
                        tetrimino_copy(&tetrimino, &temp_tetrimino); // Revert tetrimino position
                        tetrimino_status = TETRIMINO_REFRESH;
                    }
                } else if (controller_current_buttons & SNES_BUTTON_R) {
                    tetrimino.piece++;
                    if (tetrimino.piece >= TETRIMINO_COUNT) {
                        tetrimino.piece = 0;
                    }
                    tetrimino.shape_offset = tetrimino_shape_offset_lut[tetrimino.piece][tetrimino.rotation];
                    tetrimino_status = TETRIMINO_REFRESH;

                }
                if (controller_current_buttons & SNES_BUTTON_L) {
                    tetrimino.piece--;
                    if (tetrimino.piece >= TETRIMINO_COUNT) {
                        tetrimino.piece = TETRIMINO_COUNT - 1;
                    }
                    tetrimino.shape_offset = tetrimino_shape_offset_lut[tetrimino.piece][tetrimino.rotation];
                    tetrimino_status = TETRIMINO_REFRESH;
                }
//                if (controller_current_buttons & SNES_BUTTON_UP) {
//                    if (matrix_move_tetrimino(&matrix, &tetrimino, MOVE_UP) == MATRIX_REFRESH) {
//                    }
//                }

                // Handle down button press only if in normal play state
                if (game.play_state == PLAY_STATE_NORMAL) {
                    // If down button was previously pressed and is now released
                    if (controller_previous_buttons & SNES_BUTTON_DOWN
                            && !(controller_current_buttons & SNES_BUTTON_DOWN)) {
                        game.drop_time_delay = game.drop_time_normal_delay;
                        game.soft_drop_flag = 0;
                        game.soft_drop_lines = 0;
                        tetrimino_status = TETRIMINO_REFRESH;
                    } else if (controller_current_buttons & SNES_BUTTON_DOWN) {
                        game.drop_time_delay = game.drop_time_soft_drop_delay;
                        game.soft_drop_flag = 1;
                        tetrimino_status = TETRIMINO_REFRESH;
                    }
                }

//                } else {
//                    game.drop_time_delay = game.drop_time_normal_delay;
//                    game.soft_drop_flag = 0;
//                    game.soft_drop_lines = 0;
//                }

                if (controller_current_buttons & SNES_BUTTON_LEFT) {
                    matrix_move_tetrimino(&matrix, &tetrimino, MOVE_LEFT);

                }
                if (controller_current_buttons & SNES_BUTTON_RIGHT) {
                    matrix_move_tetrimino(&matrix, &tetrimino, MOVE_RIGHT);
                }
                if (controller_current_buttons & SNES_BUTTON_Y) {
//                    game.drop_time_delay += 25000;
                    game.level++;
                } else if (controller_current_buttons & SNES_BUTTON_X) {
//                    game.drop_time_delay -= 25000;
//                    if (game.drop_time_delay < 25000) {
//                        game.drop_time_delay = 25000;
//                    }
                    if (game.level > 0) {
                        game.level--;
                    }
#if DEBUG_OUTPUT
                    printf("Level: %ld\n", game.level);
#endif
                }
                if (matrix_status != MATRIX_REFRESH) {
                    // Revert tetrimino position and refresh matrix
                    matrix_status = matrix_add_tetrimino(&matrix, &tetrimino);
                    if (matrix_status == MATRIX_WALL_COLLISION || matrix_status == MATRIX_REACHED_BOTTOM) {
                        tetrimino_copy(&tetrimino, &temp_tetrimino);
                        matrix_copy(&matrix, &temp_matrix);
                    }
                } else if (tetrimino_status == TETRIMINO_REFRESH) {
                    matrix_status = matrix_add_tetrimino(&matrix, &tetrimino);
                    if (matrix_status == MATRIX_WALL_COLLISION || matrix_status == MATRIX_REACHED_BOTTOM) {
                        tetrimino_copy(&tetrimino, &temp_tetrimino);
                        matrix_copy(&matrix, &temp_matrix);
                    }
                }
            } // end if controller_status == SNES_CONTROLLER_STATE_CHANGE

            if (game.play_state == PLAY_STATE_NORMAL) {
                // Check if tetrimino is clear to continue dropping down during half-second before lock period
                matrix_copy(&temp_matrix, &matrix); // Save current matrix state
                if (util_time_expired_delay(game.drop_time_start, game.drop_time_delay)) {
                    if (tetrimino.y > 0) {
                        tetrimino.y--;
                    }

                    matrix_status = matrix_add_tetrimino(&matrix, &tetrimino);
                    if (matrix_status == MATRIX_WALL_COLLISION) {
                        matrix_copy(&matrix, &temp_matrix);
                    } else if (matrix_status == MATRIX_OUT_OF_BOUNDS) {
                        matrix_copy(&matrix, &temp_matrix);
                    } else if (matrix_status == MATRIX_REACHED_BOTTOM) {
                        matrix_copy(&matrix, &temp_matrix);
//                        matrix_status = matrix_add_tetrimino(&matrix, &tetrimino);
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
                            tetrimino.y++; // Revert tetrimino y position
                        }
                        // Edge case handling: Long bar reached to bottom of matrix, transition to lock state
                        if (tetrimino.y == 0) {
                            game.play_state = PLAY_STATE_HALF_SECOND_B4_LOCK;
                            game.lock_time_start = TIM2->CNT;
                        }
                        game.drop_time_start = TIM2->CNT;
                        if (game.soft_drop_flag) {
                            game.soft_drop_lines++;
                        }
                    }
                }
            } else {
                if (game.play_state == PLAY_STATE_HALF_SECOND_B4_LOCK) {
                    // Check if tetrimino still can fall down unobstructed, if so, revert to normal play state
                    tetrimino_copy(&temp_tetrimino, &tetrimino);
                    matrix_copy(&temp_matrix, &matrix);
                    if (temp_tetrimino.y > 0) {
                        temp_tetrimino.y--;
                    }
                    matrix_status = matrix_add_tetrimino(&temp_matrix, &temp_tetrimino);
                    if (matrix_status != MATRIX_REACHED_BOTTOM && temp_tetrimino.y > 0
                            && matrix_check_collision(&temp_matrix, &temp_tetrimino) == MATRIX_OK) {
                        // No collision detected, revert to normal play state
                        game.play_state = PLAY_STATE_NORMAL;
                        game.drop_time_start = TIM2->CNT;
                    } else {
//                        if (util_time_expired_delay(game.lock_time_start, game.lock_time_delay)) {

                        // According to documentation, the locking delay is equal to drop delay
                        if (util_time_expired_delay(game.lock_time_start, game.drop_time_delay)) {
                            game.play_state = PLAY_STATE_LOCKED;
                        }
                    }
                }
            }

            if (game.play_state == PLAY_STATE_LOCKED) {
                // Merge playfield with stack & palette
                matrix_status = merge_with_stack(&matrix, &tetrimino);
                matrix_reset_playfield(&matrix);
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
                    printf("stack values:\n");
                    for (int i = 0; i < MATRIX_DATA_SIZE; i++) {
                        printf("matrix.stack[%d] = \"0x%08lX\";\n", i, matrix.stack[i]);
                    }
                    printf("palette1 values:\n");
                    for (int i = 0; i < MATRIX_DATA_SIZE; i++) {
                        printf("matrix.palette1[%d] = \"0x%08lX\";\n", i, matrix.palette1[i]);
                    }
                    printf("palette2 values:\n");
                    for (int i = 0; i < MATRIX_DATA_SIZE; i++) {
                        printf("matrix.palette2[%d] = \"0x%08lX\";\n", i, matrix.palette2[i]);
                    }
                }
#endif
                if (lines_to_be_cleared) {
                    game.play_state = PLAY_STATE_LINE_CLEAR;
                    matrix.line_clear_bitmap = lines_to_be_cleared;
                    matrix_line_clear_start(&matrix, CLEAR_LINE_DELAY);
                    if (util_bit_count(lines_to_be_cleared) == 4) {
                        matrix.tetris_flag = 1;
                    }
                } else {
                    if (game.soft_drop_flag) {
                        game.score += game.soft_drop_lines;
                        game.soft_drop_lines = 0;
                        game.soft_drop_flag = 0;
                        game.drop_time_delay = game.drop_time_normal_delay;
                    }
                    game.play_state = PLAY_STATE_NEXT_TETRIMINO;
                }
            }

            if (game.play_state == PLAY_STATE_LINE_CLEAR) {
                if (matrix_line_clear_animate(&matrix, lines_to_be_cleared)) { // Is line clear complete?
                    if (game.soft_drop_flag) {
                        game.score += game.soft_drop_lines;
                        game.soft_drop_lines = 0;
                    }
                    // Update the score based on the number of lines cleared and game level
                    game.score += tetris_calculate_score(util_bit_count(lines_to_be_cleared), game.level);

                    // Update game statistics
                    if (util_bit_count(lines_to_be_cleared) == 1) {
                        game.stats.singles++;
                    } else if (util_bit_count(lines_to_be_cleared) == 2) {
                        game.stats.doubles++;
                    } else if (util_bit_count(lines_to_be_cleared) == 3) {
                        game.stats.triples++;
                    } else if (util_bit_count(lines_to_be_cleared) == 4) {
                        game.stats.tetrises++;
                    }

                    // Reposition blocks after clearing lines
                    matrix_reposition_blocks(&matrix, lines_to_be_cleared);
                    game.play_state = PLAY_STATE_NEXT_TETRIMINO;  // Move to next tetrimino
                    game.drop_time_start = TIM2->CNT;
                    game.lines += util_bit_count(lines_to_be_cleared);
                    lines_to_be_cleared = 0;
                    matrix.tetris_flag = 0;
                    game.soft_drop_flag = 0;
                    game.drop_time_delay = game.drop_time_normal_delay;
                    if (game.lines >= game.lines_to_next_level) {
                        game.play_state = PLAY_STATE_TRANSITION_LEVEL;
                    }
                }
            }

            // Transition to the next level if line clear count is met
            if (game.play_state == PLAY_STATE_TRANSITION_LEVEL) {
                game.level++;
                game.lines_to_next_level = 10 * (game.level + 1);
                game.drop_time_normal_delay = tetrimino_drop_speed(game.level);
                game.drop_time_soft_drop_delay = game.drop_time_normal_delay / 20;
                game.drop_time_delay = game.drop_time_normal_delay;
                game.play_state = PLAY_STATE_NEXT_TETRIMINO;
                game.drop_time_start = TIM2->CNT;
            }

            if (game.play_state == PLAY_STATE_NEXT_TETRIMINO) {
                matrix_copy(&temp_matrix, &matrix); // Save current matrix state
                tetrimino_status = tetrimino_next(&tetrimino);
                if (tetrimino_status == TETRIMINO_OK) {
                    tetris_statistics.tetriminos_frequency[tetrimino.piece]++;
                    tetris_statistics.tetriminos_spawned++;
                    matrix_status = matrix_add_tetrimino(&matrix, &tetrimino);
                    if (matrix_status == MATRIX_COLLISION_DETECTED) { // Collision detected on boundary
                        game.play_state = PLAY_STATE_TOP_OUT;

                    } else {
                        matrix_status = matrix_check_collision(&matrix, &tetrimino);
                        if (matrix_status == MATRIX_STACK_COLLISION) { // Topped out
                            // Restore previous matrix state
                            matrix_copy(&matrix, &temp_matrix);
                            game.play_state = PLAY_STATE_TOP_OUT;
                        } else {
                            game.play_state = PLAY_STATE_NORMAL;
                            game.drop_time_start = TIM2->CNT;
                        }
                    }
                }
            }

            rendering_status = renderer_render(&renderer, &matrix, &tetrimino, &game);
            if (rendering_status == RENDERER_UPDATED) {
                render_count++;
            }

            if (game.play_state == PLAY_STATE_TOP_OUT) {
                ui_display_top_out();
                renderer_top_out_start(&renderer);
                game.state = GAME_STATE_GAME_ENDED;
            }

            // Save previous controller button state
            controller_previous_buttons = controller_current_buttons;

            // TODO: Update UI
            fps_time_diff = util_time_diff_us(fps_time_last_update, TIM2->CNT);
            if (fps_time_diff >= 500000 && game.play_state != PLAY_STATE_TOP_OUT) {
                fps_start_count = fps_end_count;
                fps_end_count = render_count;
                fps_time_last_update = TIM2->CNT;
                ui_display_fps(fps_start_count, fps_end_count, fps_time_diff);
//                ui_display_game_info(&game);
                ui_display_game_progress(&game);
            }
            break;

            /* ------------------------- PAUSE MENU ------------------------ */
        case GAME_STATE_PAUSE:
            // TODO: Display pause menu
            break;

            /* -------------------------- GAME OVER ------------------------ */
        case GAME_STATE_GAME_ENDED:
            if (renderer_top_out_animate(&renderer) == RENDERER_ANIMATION_DONE) {

                // Save score if is better than a high score

                game.state = GAME_STATE_GAME_OVER_WAIT;

                // Flush the buffer
                ring_buffer_flush(&controller_buffer);
                // Persist settings and high scores by writing them to EEPROM
//                eeprom_write_settings(&eeprom, &settings);
//                eeprom_write_high_scores(&eeprom, high_score_ptrs);
            }
            break;

            /* ---------------------- GAME OVER WAIT ---------------------- */
        case GAME_STATE_GAME_OVER_WAIT:

            if (ring_buffer_dequeue(&controller_buffer, &controller_current_buttons) == true) {
                if (controller_current_buttons & SNES_BUTTON_START) {
                    game.state = GAME_STATE_MENU;
                    menu.ui_status = UI_MENU_DRAW;
//                    game.state = GAME_STATE_PREPARE_GAME;
                    ssd1306_Fill(Black);
                    ssd1306_UpdateScreen();
                    break;
                }
            }
            if (util_time_expired_delay(press_start_timer_start, 500000)) { // 500ms
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

            /* ------------------------ HIGH SCORES ------------------------ */
        case GAME_STATE_HIGH_SCORE:
            // TODO: Display high scores
            if (ring_buffer_dequeue(&controller_buffer, &controller_current_buttons) == true) {
                if (controller_current_buttons & (SNES_BUTTON_START | SNES_BUTTON_B | SNES_BUTTON_Y)) {
                    game.state = GAME_STATE_MENU;
                    menu.ui_status = UI_MENU_DRAW;
                    ssd1306_Fill(Black);
                    ssd1306_UpdateScreen();
                    break;
                }
            }
            ui_display_high_scores(high_score_ptrs, NULL);
            break;

            /* ------------------------ SETTINGS MENU ---------------------- */
        case GAME_STATE_SETTINGS:
            // TODO: Display settings menu
            break;

            /* ------------------------ TEST FEATURE ------------------------ */
        case GAME_STATE_TEST_FEATURE:
            /* Developer test code START */
//            ui_display_high_scores(high_score_ptrs, NULL);
//            rendering_status = renderer_test_render(&renderer);
//#if DEBUG_OUTPUT
//            if (rendering_status == RENDERER_UPDATED) {
//                render_count++;
//            }
//#endif
//            if(main_menu == 0)
//            {
//                ui_main_menu_selection();
//                main_menu = 1;
//            }
//            if (ring_buffer_dequeue(&controller_buffer, &controller_current_buttons) == true) {
//                if (controller_current_buttons & SNES_BUTTON_DOWN) {
//                    if (array_position > 1) {
//                        array_position = 1;
//                    } else {
//                        array_position++;
//                    }
//
//                    if (cursor_position < 2) {
//                        cursor_position++;
//                    } else {
//                        cursor_position = 2;
//                    }
//                }
//                if (controller_current_buttons & SNES_BUTTON_UP) {
//                    if (array_position < 0) {
//                        array_position = 0;
//                    } else {
//                        array_position--;
//                    }
//
//                    if (cursor_position > 0) {
//                        cursor_position--;
//                    } else {
//                        cursor_position = 0;
//                    }
//                }
//            ssd1306_SetCursor(32, 14);
//            ssd1306_WriteString(main_menu_list[array_position], Font_7x10, White);
//
//            ssd1306_SetCursor(32, 30);
//            ssd1306_WriteString(main_menu_list[array_position + 1], Font_7x10, White);
//
//            ssd1306_SetCursor(32, 48);
//            ssd1306_WriteString(main_menu_list[array_position + 2], Font_7x10, White);
//
//
//            ssd1306_SetCursor(10, select_arrow_locations[cursor_position]);
//            ssd1306_WriteString(">", Font_6x8, White);
//
//            ssd1306_UpdateScreen();
//            }
            /* Developer test code END */
            break;

            /* ----------------------- UNKNOWN STATES ---------------------- */
        default:
            // TODO: Handle unknown states (fail-safe)
            break;
        } // end switch

        game_loop_count++;
        led_indicator(&hb_led);
        led_indicator(&rj45_led);
    } // end for loop
} // end game_loop
