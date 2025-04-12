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

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "ssd1306.h" // for OLED screen
#include "ui.h"
#include "splash_bitmap.h"
#include "util.h"
#include "tetris.h"
#include "eeprom.h"


ui_stats_t ui_stats;
ui_stats_t ui_high_score;

/**
 * @brief  Initialize OLED display
 * @param  None
 * @retval None
 */
void ui_init(tetris_statistics_t *stats) {
    ssd1306_Init();
    memset(&ui_stats, 0, sizeof(ui_stats_t));
    ui_stats.stats = stats;

    // High score
    memset(&ui_high_score, 0, sizeof(ui_stats_t));
}

/**
 * @brief  Reset UI statistics
 * @param  None
 * @retval None
 */
void ui_reset_ui_stats() {
    ui_stats.animate_start_time = TIM2->CNT;
    ui_stats.animate_delay = UI_STATS_DELAY;
    ui_stats.animate_frame = 0;

    ui_high_score.animate_start_time = TIM2->CNT;
    ui_high_score.animate_delay = UI_STATS_DELAY;
    ui_high_score.animate_frame = 0;
}

void ui_splash_screen() {
//    ssd1306_SetContrast(50); // to Dr. Butler - what does this do? i tried to change value but it seems to do nothing
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
void ui_main_menu_selection(void) {
    // TODO: Display main menu selection
}

/**
 * @brief  Display game progress
 * @param  game_t: pointer to game_t struct
 * @retval None
 */
void ui_display_game_progress(game_t *game) {
    char buffer[32];
    memset(buffer, 0, sizeof(buffer));

    ui_display_game_info(game);
    // Display lines cleared, game score, level, tetrimino count and time elapsed
    if (util_time_expired_delay(ui_stats.animate_start_time, ui_stats.animate_delay)) {
        ui_stats.animate_start_time = TIM2->CNT;
        ui_stats.animate_delay = UI_STATS_DELAY;
        ui_stats.animate_frame++;
        if (ui_stats.animate_frame >= UI_STATS_NUM_FRAMES) {
            ui_stats.animate_frame = 0;
        }
        // clear previous frame
        ssd1306_FillRectangle(0, 24, 128, 55, Black);

        // display current frame
        ssd1306_SetCursor(0, 22);

        switch (ui_stats.animate_frame) {
        case 0:
            ssd1306_SetCursor(0, 32);
            snprintf(buffer, 32, "T: %d", ui_stats.stats->tetriminos_frequency[TETRIMINO_T]);
            ssd1306_WriteString(buffer, Font_6x8, White);

            ssd1306_SetCursor(60, 32);
            snprintf(buffer, 32, "J: %d", ui_stats.stats->tetriminos_frequency[TETRIMINO_J]);
            ssd1306_WriteString(buffer, Font_6x8, White);

            ssd1306_SetCursor(0, 42);
            snprintf(buffer, 32, "Z: %d", ui_stats.stats->tetriminos_frequency[TETRIMINO_Z]);
            ssd1306_WriteString(buffer, Font_6x8, White);

            ssd1306_SetCursor(60, 42);
            snprintf(buffer, 32, "O: %d", ui_stats.stats->tetriminos_frequency[TETRIMINO_O]);
            ssd1306_WriteString(buffer, Font_6x8, White);
            break;

        case 1:
            ssd1306_SetCursor(0, 32);
            snprintf(buffer, 32, "S: %d", ui_stats.stats->tetriminos_frequency[TETRIMINO_S]);
            ssd1306_WriteString(buffer, Font_6x8, White);

            ssd1306_SetCursor(60, 32);
            snprintf(buffer, 32, "L: %d", ui_stats.stats->tetriminos_frequency[TETRIMINO_L]);
            ssd1306_WriteString(buffer, Font_6x8, White);

            ssd1306_SetCursor(0, 42);
            snprintf(buffer, 32, "I: %d", ui_stats.stats->tetriminos_frequency[TETRIMINO_I]);
            ssd1306_WriteString(buffer, Font_6x8, White);

            ssd1306_SetCursor(60, 42);
            snprintf(buffer, 32, "Total: %d", ui_stats.stats->tetriminos_spawned);
            ssd1306_WriteString(buffer, Font_6x8, White);
            break;

        case 2:
            ssd1306_SetCursor(0, 32);
            snprintf(buffer, 32, "Single: %d", game->stats.singles);
            ssd1306_WriteString(buffer, Font_6x8, White);

            ssd1306_SetCursor(60, 32);
            snprintf(buffer, 32, "Double: %d", game->stats.doubles);
            ssd1306_WriteString(buffer, Font_6x8, White);

            ssd1306_SetCursor(0, 42);
            snprintf(buffer, 32, "Triple: %d", game->stats.triples);
            ssd1306_WriteString(buffer, Font_6x8, White);

            ssd1306_SetCursor(60, 42);
            snprintf(buffer, 32, "Tetris: %d", game->stats.tetrises);
            ssd1306_WriteString(buffer, Font_6x8, White);
            break;

        default:
            break;
        }
        ssd1306_UpdateScreen();
    }
}

/**
 * @brief  Display game over screen
 * @param  None
 * @retval None
 */
void ui_game_over_screen(game_t *game, ui_stats_t *ui_stats) {
    // TODO: Display game over screen with final score, level, and time
    ui_display_game_info(game);
    // TODO: Display high score if applicable, and prompt for name entry

}

/**
 * @brief  Display highscores
 * @param  high_scores: array of high scores to display
 * @param  curr_score: an optional additional score to be displayed. NULL if not desired
 * @retval None
 */
void ui_display_high_scores(game_high_score_t *high_scores[], game_t *game) {
    // Only update screen if its time to switch
    if (util_time_expired_delay(ui_high_score.animate_start_time, ui_high_score.animate_delay)) {
        char buffer[32];
        memset(buffer, 0, sizeof(buffer));

        ui_high_score.animate_start_time = TIM2->CNT;
        ui_high_score.animate_delay = UI_STATS_DELAY;

        // Figure out which header to display
        if (ui_high_score.animate_frame == 0) {
            snprintf(buffer, 32, "  NAME   SCORE  LEVEL");
        }
        else {
            snprintf(buffer, 32, "  NAME   SCORE  LINES");
        }

        // Display the header
        ssd1306_SetCursor(1, 2);
        ssd1306_WriteString(buffer, Font_6x8, White);

        // Display high scores
        for (int i = 0; i < EEPROM_NUM_HIGH_SCORES; i++) {
            // Display name
            ssd1306_SetCursor(1, 11 + i * 9);
            snprintf(buffer, 32, "%d %s", i + 1, high_scores[i]-> name);
            ssd1306_WriteString(buffer, Font_6x8, White);

            // Display score and level/lines
            ssd1306_SetCursor(55, 11 + i * 9);
            if (ui_high_score.animate_frame == 0) {
                snprintf(buffer, 32, "%06ld %02ld", high_scores[i]->score, high_scores[i]->level);
            }
            else {
                snprintf(buffer, 32, "%06ld %02ld", high_scores[i]->score, high_scores[i]->level);
            }
            ssd1306_WriteString(buffer, Font_6x8, White);
        }

        // If given a current game state, display it too
        if (game != NULL) {
            ssd1306_SetCursor(1, 2 + 6 * 9);
            if (ui_high_score.animate_frame == 0) {
                snprintf(buffer, 32, "  YOU    %06ld %02ld", game->score, game->level);
            }
            else {
                snprintf(buffer, 32, "  YOU    %06ld %03ld", game->score, game->lines);
            }
            ssd1306_WriteString(buffer, Font_6x8, White);
        }

        // switch the frame to display next time
        ui_high_score.animate_frame ^= (1 << 0);

        ssd1306_UpdateScreen();
    }
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
    sprintf(game_info_str, "%06ld", game->score);
    ssd1306_WriteString(game_info_str, Font_11x18, White);

    sprintf(game_info_str, "LIN: %ld", game->lines);
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
    // clear statistics area
    ssd1306_FillRectangle(0, 24, 128, 55, Black);
    // display top out message
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
