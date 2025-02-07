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
#include "cmsis_os.h" // for "sleep" function
#include "splash_bitmap.h"

/**
 * @brief  Initialize OLED display
 * @param  None
 * @retval None
 */
void ui_init() {
    ssd1306_Init();
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
        osDelay(350);
    }

    bitmap_number++;
    for (int i = 60; i <= 90; i += 10) { // Moves right}
        ssd1306_SetCursor(i, 0);
        ssd1306_WriteString("     ", Font_16x26, White);
        ssd1306_DrawBitmap(i, 0, tetrimino_allArray[bitmap_number], 13, 20, White);
        ssd1306_UpdateScreen();
        ssd1306_SetCursor((i - 5), 0); // to clean up after displaying
        ssd1306_WriteString("     ", Font_16x26, White);
        osDelay(350);
    }

    bitmap_number++;

    for (int i = 90; i >= 66; i -= 10) { // Moves right to center again
        ssd1306_WriteString("  ", Font_16x24, White);
        ssd1306_SetCursor(i, 0);
        ssd1306_DrawBitmap(i, 0, tetrimino_allArray[bitmap_number], 20, 13, White);
        ssd1306_UpdateScreen();
        osDelay(350);
    }

    for (int i = 0; i < 30; i += 10) { // Moves down
        if (i == 0) {
            ssd1306_WriteString("  ", Font_16x24, White);
        }
        ssd1306_WriteString("  ", Font_11x18, White);
        ssd1306_SetCursor(66, i);
        ssd1306_DrawBitmap(66, i, tetrimino_allArray[bitmap_number], 20, 13, White);
        ssd1306_UpdateScreen();
        osDelay(350);
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
            osDelay(50);
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
        osDelay(50);
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
//        osDelay(600);
//        ssd1306_SetCursor(30, 55);
//        ssd1306_WriteString("                ", Font_6x8, White); // erasing
//        ssd1306_UpdateScreen();
//        osDelay(600);
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
    // TODO: Calculate and display frames per second
    uint32_t fps = ((end_count - start_count) * 10000000) / time_us;
    ssd1306_SetCursor(0, 55);
    char fps_str[32];
    memset(fps_str, 0, sizeof(fps_str));
    sprintf(fps_str, "fps:%d.%d   ", (int) (fps / 10), (int) (fps % 10));
    ssd1306_WriteString(fps_str, Font_6x8, White);
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
