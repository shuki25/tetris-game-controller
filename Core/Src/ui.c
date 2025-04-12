/**
 ******************************************************************************
 * @file           : ui.c
 * @author         : Ragde Primero, Dr. Joshua Butler
 * @date           : Dec 9, 2024
 * @brief          : OLED Display UI functions
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

//TO DO FOR TOMORROW
// - Add state machines for ui, (FIRST_TIME_DRAW, WAITING_STATE, CONTROLLER_DETECTED)
// - Add state in UI struct in header! Also add emun for states in ui header
// - For controller_detected, look at Prof. Butler code for game state with play state especially matrix movement and try to write code like him in controller modify


#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "ssd1306.h" // for OLED screen
#include "ui.h"
#include "splash_bitmap.h"

//uint8_t select_arrow_locations[3] = { 14, 30, 46 };
const char *menu_title_list[] = {
    "Main Menu", // Start Menu
    "Game Modes", // Game mode menu
    "Paused", // Pause menu
    "Settings" // Settings menu
};

const char *menu_list[][5] = {
    {"Play game", "High Score", "Settings", "Credits"}, // Start Menu
    {"Classic", "Placeholder", "placeholder"}, // Game mode menu
    {"Continue", "Restart", "Quit"}, // Pause menu
    {"Brightness", "Debug", "Reset High Score", "Scoreboard ID"} // Settings menu
};

const uint8_t select_arrow_locations[3] = { 14, 30, 46 };


/**
 * @brief  Initialize OLED display
 * @param  None
 * @retval None
 */
void ui_init() {
    ssd1306_Init();
}

void ui_menu_init(ui_menu_t menu) {
//    memset(menu, 0, sizeof(*menu));
    menu.menu_id = 0;
    menu.current_selection_id = 0;
    menu.cursor_selection_id = 0;
    menu.is_cursor_on = 0;
    menu.cursor_timeout = 0;
    menu.ui_status = UI_MENU_DRAW;
    menu.ui_menu_list_size = 0;
    menu.offset_num = 0;
}
void ui_menu_id_set(ui_menu_t * menu, int menuID)
{
    menu->menu_id = menuID;
    menu->ui_menu_list_size = (sizeof(menu_list[menu->menu_id]) / sizeof(menu_list[menu->menu_id][0])-2);
}

void ui_splash_screen() {
    ssd1306_Fill(Black);

    ssd1306_SetCursor(5, 8);
    ssd1306_WriteString("Imagine", Font_7x10, White);

    ssd1306_SetCursor(35, 27);
    ssd1306_WriteString("RI ", Font_16x24, White);

    ssd1306_SetCursor(59, 55); // x y
    ssd1306_WriteString("presents...", Font_6x8, White);

    ssd1306_UpdateScreen();
    int bitmap_number = 0;

    for (int i = 100; i >= 60; i -= 10) { // Moves left
        ssd1306_SetCursor(i, 0);
        ssd1306_WriteString("    ", Font_16x24, White);
        ssd1306_DrawBitmap(i, 0, tetrimino_allArray[bitmap_number], 20, 13, White);
        ssd1306_UpdateScreen();
        HAL_Delay(250);
    }

    bitmap_number++;
    for (int i = 60; i <= 90; i += 10) { // Moves right}
        ssd1306_SetCursor(i, 0);
        ssd1306_WriteString("     ", Font_16x26, White);
        ssd1306_DrawBitmap(i, 0, tetrimino_allArray[bitmap_number], 13, 20, White);
        ssd1306_UpdateScreen();
        ssd1306_SetCursor((i - 5), 0); // to clean up after displaying
        ssd1306_WriteString("     ", Font_16x26, White);
        HAL_Delay(250);
    }

    bitmap_number++;

    for (int i = 90; i >= 66; i -= 10) { // Moves right to center again
        ssd1306_WriteString("  ", Font_16x24, White);
        ssd1306_SetCursor(i, 0);
        ssd1306_DrawBitmap(i, 0, tetrimino_allArray[bitmap_number], 20, 13, White);
        ssd1306_UpdateScreen();
        HAL_Delay(250);
    }

    for (int i = 0; i < 30; i += 10) { // Moves down
        if (i == 0) {
            ssd1306_WriteString("  ", Font_16x24, White);
        }
        ssd1306_WriteString("  ", Font_11x18, White);
        ssd1306_SetCursor(66, i);
        ssd1306_DrawBitmap(66, i, tetrimino_allArray[bitmap_number], 20, 13, White);
        ssd1306_UpdateScreen();
        HAL_Delay(250);
    }

    for (int i = 0; i < 8; i++) { // Light flashing
        if (i % 2 == 0) {
            ssd1306_Fill(White);

            ssd1306_SetCursor(5, 8);
            ssd1306_WriteString("Imagine", Font_7x10, Black);

            ssd1306_SetCursor(35, 27);
            ssd1306_WriteString("RI ", Font_16x24, Black);

            ssd1306_DrawBitmap(66, 27, tetrimino_allArray[bitmap_number], 20, 13, Black);

            ssd1306_SetCursor(59, 55); // x y
            ssd1306_WriteString("presents...", Font_6x8, Black);

            ssd1306_UpdateScreen();
            HAL_Delay(40);
        }
        ssd1306_Fill(Black);

        ssd1306_SetCursor(5, 8);
        ssd1306_WriteString("Imagine", Font_7x10, White);

        ssd1306_SetCursor(35, 27);
        ssd1306_WriteString("RI ", Font_16x24, White);

        ssd1306_DrawBitmap(66, 27, tetrimino_allArray[bitmap_number], 20, 13, White);

        ssd1306_SetCursor(59, 55); // x y
        ssd1306_WriteString("presents...", Font_6x8, White);

        ssd1306_UpdateScreen();
        HAL_Delay(40);
//        HAL_Delay(50);
    }

    ssd1306_Fill(Black);
    ssd1306_DrawBitmap(33, 11, tetrimino_allArray[4], 63, 16, White); // Tetris logo
    ssd1306_DrawBitmap(24, 0, tetrimino_allArray[3], 75, 55, White); // T DOWN outline but bigger
    ssd1306_UpdateScreen();

    // Blinking press start is handled in game_loop (non-blocking)

//    while (1)  // 'Press Start' blinking cycle
//    {
//        ssd1306_SetCursor(30, 55);
//        ssd1306_WriteString("Press start", Font_6x8, White);
//        ssd1306_UpdateScreen();
//        HAL_Delay(600);
//        ssd1306_SetCursor(30, 55);
//        ssd1306_WriteString("                ", Font_6x8, White); // erasing
//        ssd1306_UpdateScreen();
//        HAL_Delay(600);
//    }

}

/**
 * @brief  Display main menu selection
 * @param  None
 * @retval Menu selection (use enum constants for each menu item)
 */
void ui_main_menu_selection(ui_menu_t * menu) {
    // TODO: Display main menu selection
    if(menu->ui_status == UI_MENU_DRAW)
    {

        ssd1306_Fill(Black);
        frame_maker();
        ssd1306_SetCursor(34, 0);
        ssd1306_WriteString(" ", Font_6x8, White);
        ssd1306_SetCursor(36, 0);
        ssd1306_WriteString(menu_title_list[menu->menu_id], Font_6x8, White);

        ssd1306_SetCursor(32, 14);
        ssd1306_WriteString(menu_list[menu->menu_id][menu->offset_num], Font_7x10, White);

        ssd1306_SetCursor(32, 30);
        ssd1306_WriteString(menu_list[menu->menu_id][menu->offset_num+1], Font_7x10, White);

        ssd1306_SetCursor(32, 46);
        ssd1306_WriteString(menu_list[menu->menu_id][menu->offset_num+2], Font_7x10, White);

        if(menu->is_cursor_on == 1)
        {
            if(menu->cursor_selection_id > 2)
            {
                menu->cursor_selection_id = 2;
                ssd1306_SetCursor(23, select_arrow_locations[2]);
                ssd1306_WriteString(">", Font_6x8, White);
                ssd1306_UpdateScreen();

            }
            else if(menu->cursor_selection_id < 0)
            {
                menu->cursor_selection_id = 0;
                ssd1306_SetCursor(23, select_arrow_locations[0]);
                ssd1306_WriteString(">", Font_6x8, White);
                ssd1306_UpdateScreen();
            }

            ssd1306_SetCursor(23, select_arrow_locations[menu->cursor_selection_id]);
            ssd1306_WriteString(">", Font_6x8, White);
            ssd1306_UpdateScreen();

            menu->is_cursor_on = 0;
        }
        else{
            if(menu->cursor_timeout == 5000)
            {
                menu->is_cursor_on = 1;
            }
            else{
                menu->cursor_timeout += 1;
            }
        }
        ssd1306_UpdateScreen();
        menu->ui_status = UI_WAITING_STATE;
    }

}

//void menu_scroll_draw(ui_menu_t * menu, uint8_t offset)
//{
//
//}

void frame_maker(void) {

    ssd1306_DrawBitmap(0, 0, tetrimino_allArray[5], 128, 64, White);
}

/**
 * @brief  Display game progress
 * @param  None
 * @retval None
 */
void ui_game_progress() {
    // TODO: Display lines cleared, game score, level, tetrimino count and time elapsed
}

/**
 * @brief  Display game over screen
 * @param  None
 * @retval None
 */
void ui_game_over_screen() {
    // TODO: Display game over screen with final score, level, and time

    // TODO: Display high score if applicable, and prompt for name entry

}

/**
 * @brief  Display settings menu
 * @param  None
 * @retval None
 */
void ui_settings_menu() {
    // TODO: Display settings menu (brightness, reset high score, scoreboard device identifier, etc.)
}

/**
 * @brief  Display Frames Per Second (FPS) in lower left corner
 * @param  None
 * @retval None
 */
void ui_display_fps(uint32_t start_count, uint32_t end_count, uint32_t time_us) {
    uint32_t fps = ((end_count - start_count) * 10000000) / time_us;
    ssd1306_SetCursor(0, 55);
    char fps_str[32];
    memset(fps_str, 0, sizeof(fps_str));
    sprintf(fps_str, "fps:%d.%d   ", (int) (fps / 10), (int) (fps % 10));
    ssd1306_WriteString(fps_str, Font_6x8, White);
    ssd1306_UpdateScreen();
}

void ui_display_game_info(game_t *game) {
    uint8_t x;

    char game_info_str[32];
    memset(game_info_str, 0, sizeof(game_info_str));
    ssd1306_SetCursor(0, 2);
    sprintf(game_info_str, "%07ld", game->score);
    ssd1306_WriteString(game_info_str, Font_11x18, White);

    sprintf(game_info_str, "%ld", game->lines);
    x = 128 - strlen(game_info_str) * 6;
    ssd1306_SetCursor(x, 0);
    ssd1306_WriteString(game_info_str, Font_6x8, White);

    sprintf(game_info_str, "LVL: %ld", game->level);
    x = 128 - strlen(game_info_str) * 6;
    ssd1306_SetCursor(x, 10);
    ssd1306_WriteString(game_info_str, Font_6x8, White);

    ssd1306_UpdateScreen();
}

void ui_display_top_out() {
    ssd1306_SetCursor(25, 24);
    ssd1306_WriteString("TOP OUT", Font_11x18, White);
    ssd1306_SetCursor(0, 55);
    ssd1306_WriteString("        ", Font_6x8, White);
    ssd1306_SetCursor(37, 44);
    ssd1306_WriteString("Game Over", Font_6x8, White);
    ssd1306_UpdateScreen();
}

void ui_test() {
    ssd1306_Init();
    ssd1306_SetContrast(50);
    ssd1306_Fill(Black);
    ssd1306_SetCursor(30, 0);
    ssd1306_WriteString("TEST", Font_16x26, White);
    ssd1306_UpdateScreen();
}

//void menu_player_initials(char *player_initials, uint16_t high_score, snes_controller_t *controller) {
//
//    char buffer[20];
//
//    ssd1306_Init();
//    ssd1306_Fill(Black);
//    ssd1306_SetCursor(9, 2);
//    ssd1306_WriteString("High Score", Font_11x18, White);
//    ssd1306_SetCursor(42, 20);
//    sprintf(buffer, "%04d", high_score);
//    ssd1306_WriteString(buffer, Font_11x18, White);
//    ssd1306_SetCursor(53, 40);
//    ssd1306_WriteString("___", Font_7x10, White);
//    ssd1306_DrawRectangle(0, 0, 127, 63, White);
//    ssd1306_SetCursor(36, 53);
//    ssd1306_WriteString("B = Save", Font_7x10, White);
//    ssd1306_UpdateScreen();
//
//    memset(player_initials, 0, 4);
//    memset(player_initials, '_', 3);
//
//    uint8_t is_done = 0;
//    uint8_t initial_position = 0;
//    uint8_t blink_state = 1;
//    uint8_t counter = 3;
//    uint8_t done = 0;
//
//    // Wait til no buttons are pressed
//    while (done < 2) {
//        snes_controller_read(controller);
//        if (controller->current_button_state == 0) {
//            done++;
//        }
//        HAL_Delay(100);
//    }
//    while (!is_done) {
//        snes_controller_read(controller);
//
//        if ((controller->current_button_state != controller->previous_button_state
//                || (controller->current_button_state == controller->previous_button_state))
//                && controller->current_button_state) {
//            if (controller->current_button_state & SNES_DOWN_MASK) {
//                if (player_initials[initial_position] < 'Z') {
//                    player_initials[initial_position]++;
//                    blink_state = 1;
//                    counter = 0;
//                } else {
//                    player_initials[initial_position] = 'A';
//                    blink_state = 1;
//                    counter = 0;
//                }
//            } else if (controller->current_button_state & SNES_UP_MASK) {
//                if (player_initials[initial_position] > 'A') {
//                    player_initials[initial_position]--;
//                    blink_state = 1;
//                    counter = 0;
//                } else {
//                    player_initials[initial_position] = 'Z';
//                    blink_state = 1;
//                    counter = 0;
//                }
//            } else if (controller->current_button_state & SNES_LEFT_MASK) {
//                if (initial_position > 0) {
//                    ssd1306_SetCursor(53 + (initial_position * 7), 40);
//                    ssd1306_WriteString(&player_initials[initial_position], Font_7x10, White);
//                    initial_position--;
//                    blink_state = 1;
//                    counter = 0;
//                }
//            } else if (controller->current_button_state & SNES_RIGHT_MASK) {
//                if (initial_position < 2) {
//                    ssd1306_SetCursor(53 + (initial_position * 7), 40);
//                    ssd1306_WriteString(&player_initials[initial_position], Font_7x10, White);
//                    initial_position++;
//                    blink_state = 1;
//                    counter = 0;
//                }
//            } else if (controller->current_button_state & SNES_B_MASK) {
//                is_done = 1;
//            }
//        }
//
//        if (blink_state && !counter) {
//            ssd1306_SetCursor(53 + (initial_position * 7), 40);
//            ssd1306_WriteString(&player_initials[initial_position], Font_7x10, White);
//            blink_state = 0;
//            counter = 3;
//            ssd1306_UpdateScreen();
//        } else if (!blink_state && !counter) {
//            ssd1306_SetCursor(53 + (initial_position * 7), 40);
//            ssd1306_WriteString(" ", Font_7x10, White);
//            blink_state = 1;
//            counter = 3;
//            ssd1306_UpdateScreen();
//        }
//        counter--;
//        osDelay(BLINK_DELAY * 3);
//    }
//}
//
//void menu_scoreboard_settings(uint8_t *i2c_addr, snes_controller_t *controller) {
//
//    ssd1306_Fill(Black);
//    ssd1306_SetCursor(9, 2);
//    ssd1306_WriteString("Scoreboard", Font_11x18, White);
//    ssd1306_DrawRectangle(0, 0, 127, 63, White);
//    ssd1306_SetCursor(22, 22);
//    ssd1306_WriteString("Press < > to", Font_7x10, White);
//    ssd1306_SetCursor(8, 34);
//    ssd1306_WriteString("assign I2C Addr.", Font_7x10, White);
//    ssd1306_UpdateScreen();
//
//    uint8_t current_i2c_addr = *i2c_addr;
//    if (current_i2c_addr > 0x1F) {
//        current_i2c_addr = 0x1F;
//    } else if (current_i2c_addr < 0x10) {
//        current_i2c_addr = 0x10;
//    }
//
//    char oled_buffer[20];
//    memset(oled_buffer, 0, 20);
//
//    uint8_t is_done = 0;
//    while (!is_done) {
//        snes_controller_read(controller);
//        if (controller->current_button_state == 0) {
//            is_done = 1;
//        }
//        osDelay(10);
//    }
//
//    sprintf(oled_buffer, "0x%02X", current_i2c_addr);
//    ssd1306_SetCursor(50, 48);
//    ssd1306_WriteString((char*) oled_buffer, Font_7x10, White);
//    ssd1306_UpdateScreen();
//
//    is_done = 0;
//    while (!is_done) {
//        snes_controller_read(controller);
//        if (controller->current_button_state == SNES_LEFT_MASK
//                || controller->current_button_state == SNES_RIGHT_MASK) {
//            if (controller->current_button_state == SNES_LEFT_MASK && current_i2c_addr > 0x10) {
//                current_i2c_addr--;
//            } else if (controller->current_button_state == SNES_RIGHT_MASK && current_i2c_addr < 0x1F) {
//                current_i2c_addr++;
//            }
//            sprintf(oled_buffer, "0x%02X", current_i2c_addr);
//            ssd1306_SetCursor(50, 48);
//            ssd1306_WriteString((char*) oled_buffer, Font_7x10, White);
//            ssd1306_UpdateScreen();
//        } else if (controller->current_button_state == SNES_Y_MASK) {
//            is_done = 1;
//        } else if (controller->current_button_state == SNES_B_MASK) {
//            *i2c_addr = current_i2c_addr;
//            is_done = 1;
//        }
//        osDelay(100);
//    }
//}
