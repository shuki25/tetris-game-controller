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
#include "snes_controller.h"
#include "eeprom.h"
#include "tetris.h"
#include "snes_controller.h"
#include "eeprom.h"

#define UI_STATS_NUM_FRAMES (3) // Number of frames for statistics animation
#define UI_STATS_DELAY (1500000) // Delay between statistics frames in microseconds
#define UI_HIGH_SCORE_DELAY (2500000) // Delay between high score frames in microseconds
#define UI_CURSOR_X_POS (13) // X position of the cursor
#define UI_MENU_OPTION_X_POS (23) // X position of the menu options
#define UI_MENU_OPTION_FONT (Font_7x10) // Font for menu options

// TODO: Typedef for UI status in enum (e.g. UI_OK, UI_ERROR)
typedef enum {
    UI_MENU_DRAW, UI_WAITING_STATE, UI_CONTROLLER_DETECTED, UI_LEVEL_SELECTION_DRAW, UI_LEVEL_SELECTION,
} ui_state_t;

typedef struct {
    uint8_t menu_id;
    uint8_t cursor_selection_id;
    uint8_t is_cursor_on;
    uint32_t cursor_timeout;
    uint8_t current_selection_id;
    ui_state_t ui_status;
    uint8_t ui_menu_list_size;
    uint8_t offset_num;
    uint32_t cursor_start_time;
} ui_menu_t;

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
void ui_menu_init(ui_menu_t *menu);
void ui_menu_id_set(ui_menu_t *menu, int menuID);
void ui_reset_ui_stats();
void ui_splash_screen();

//General Menu
void ui_main_menu_selection(ui_menu_t *menu);
void ui_menu_controller_move_up(ui_menu_t *menu);
void ui_menu_controller_move_down(ui_menu_t *menu);
void ui_menu_cursor_blink(ui_menu_t *menu);

//Level Selection
void ui_level_controller_move_up(uint32_t *level, ui_state_t *ui_level_selection_mode);
void ui_level_controller_move_down(uint32_t *level, ui_state_t *ui_level_selection_mode);
void ui_level_selection(uint32_t *level, ui_state_t *ui_level_selection_mode, uint8_t *ui_is_cursor_on);

void ui_test();
void ui_display_high_scores(game_high_score_t *high_scores[], game_t *game);
void frame_maker();

void ui_display_fps(uint32_t start_count, uint32_t end_count, uint32_t time_us);
void ui_display_game_progress(game_t *game);
void ui_display_game_info(game_t *game);
void ui_display_top_out();
void ui_get_initials_high_score(game_t *game, game_high_score_t *high_score,
        snes_controller_t *controller);
void ui_display_not_implemented(snes_controller_t *controller);
void ui_elapsed_time(uint32_t game_elapsed_time);
#endif /* INC_UI_H_ */
