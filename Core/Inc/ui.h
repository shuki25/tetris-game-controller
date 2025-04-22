/**
 ******************************************************************************
 * @file           : ui.h
 * @author         : josh
 * @date           : Dec 9, 2024
 * @brief          :
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

#ifndef INC_UI_H_
#define INC_UI_H_

#include "game_loop.h"
#include "tetris.h"
#include "snes_controller.h"
#include "eeprom.h"

#define UI_STATS_NUM_FRAMES (3) // Number of frames for statistics animation
#define UI_STATS_DELAY (1500000) // Delay between statistics frames in microseconds

// TODO: Typedef for UI status in enum (e.g. UI_OK, UI_ERROR)

// TODO: Typedef constants in enum for menu selection (e.g. MAIN_MENU, GAME_PROGRESS, GAME_OVER)

// Typedef for displaying statistics (e.g. game_t)
typedef struct {
    uint32_t animate_start_time;
    uint32_t animate_delay;
    uint8_t animate_frame;
    tetris_statistics_t *stats;
} ui_stats_t;

// TODO: Function prototypes for UI functions (e.g. ui_init, ui_main_menu_selection, ui_game_progress, ui_game_over_screen)
void ui_init();
void ui_reset_ui_stats();
void ui_splash_screen();
void ui_test();
void ui_display_fps(uint32_t start_count, uint32_t end_count, uint32_t time_us);
void ui_display_game_progress(game_t *game);
void ui_display_game_info(game_t *game);
void ui_display_top_out();
void ui_get_initials_high_score(game_t *game, game_high_score_t *high_score,
        snes_controller_t *controller);
#endif /* INC_UI_H_ */
