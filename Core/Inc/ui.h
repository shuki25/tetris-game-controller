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

// TODO: Typedef for UI status in enum (e.g. UI_OK, UI_ERROR)
typedef struct {
    uint8_t menu_id;
    uint8_t is_cursor_on;
    uint8_t cursor_timeout;
    uint8_t current_selection_id;
    char* menu_options; // need to put this below all otherwise, it wont work!
} ui_menu_t;

// TODO: Typedef constants in enum for menu selection (e.g. MAIN_MENU, GAME_PROGRESS, GAME_OVER)

// TODO: Function prototypes for UI functions (e.g. ui_init, ui_main_menu_selection, ui_game_progress, ui_game_over_screen)
void ui_init();
void ui_menu_init(ui_menu_t* menu);
void ui_splash_screen();
void ui_main_menu_selection();
void ui_test();
void frame_maker();

void ui_display_fps(uint32_t start_count, uint32_t end_count, uint32_t time_us);
#endif /* INC_UI_H_ */
