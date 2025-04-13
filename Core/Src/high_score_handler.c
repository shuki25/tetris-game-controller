/*
 * high_score_handler.c
 *
 *  Created on: Apr 12, 2025
 *      Author: yoel
 */

#include "high_score_handler.h"
#include "ssd1306.h"
#include "snes_controller.h"
#include <stdio.h>
#include <string.h>
#include "itm_debug.h"

uint8_t check_high_score(game_t *game, game_high_score_t **high_score) {
    uint8_t is_new_score = 0;
    if (game->score > high_score[EEPROM_NUM_HIGH_SCORES - 1]->score) {
        is_new_score = 1;
    }
    return is_new_score;
}

void update_high_score(uint32_t game_score, game_high_score_t **high_score,
        snes_controller_t *controller) {
    char player_initials_buffer[7];
    memset(player_initials_buffer, '_', sizeof(player_initials_buffer) - 1);
    player_initials_buffer[6] = '\0';
    char score_buffer[20];

    ssd1306_Init();
    ssd1306_Fill(Black);
    ssd1306_SetCursor(9, 2);
    ssd1306_WriteString("High Score", Font_11x18, White);
    ssd1306_SetCursor(31, 20);
    sprintf(score_buffer, "%06d", game_score);
    ssd1306_WriteString(score_buffer, Font_11x18, White);
    ssd1306_SetCursor(53, 40);
    ssd1306_WriteString("___", Font_7x10, White);
    ssd1306_DrawRectangle(0, 0, 127, 63, White);
    ssd1306_SetCursor(36, 53);
    ssd1306_WriteString("B = Save", Font_7x10, White);
    ssd1306_UpdateScreen();

    uint8_t is_done = 0;
    uint8_t initial_position = 0;
    uint8_t blink_state = 1;
    uint8_t counter = 3;
    uint8_t done = 0;

    // Wait til no buttons are pressed
    while (done < 2) {
        snes_controller_read(controller);
        if (controller->buttons_state == 0) {
            done++;
        }
        HAL_Delay(100);
    }

    // enter your initials
    while (!is_done) {
        snes_controller_read(controller);

        if ((controller->buttons_state != controller->previous_buttons_state
                || (controller->buttons_state
                        == controller->previous_buttons_state))
                && controller->buttons_state) {
            if (controller->buttons_state & SNES_BUTTON_DOWN) {
                if (player_initials_buffer[initial_position] < 'Z') {
                    player_initials_buffer[initial_position]++;
                    blink_state = 1;
                    counter = 0;
                } else {
                    player_initials_buffer[initial_position] = 'A';
                    blink_state = 1;
                    counter = 0;
                }
            } else if (controller->buttons_state & SNES_BUTTON_UP) {
                if (player_initials_buffer[initial_position] > 'A') {
                    player_initials_buffer[initial_position]--;
                    blink_state = 1;
                    counter = 0;
                } else {
                    player_initials_buffer[initial_position] = 'Z';
                    blink_state = 1;
                    counter = 0;
                }
            } else if (controller->buttons_state & SNES_BUTTON_LEFT) {
                if (initial_position > 0) {
                    ssd1306_SetCursor(46 + (initial_position * 7), 40);
                    ssd1306_WriteString(
                            &player_initials_buffer[initial_position],
                            Font_7x10, White);
                    initial_position--;
                    blink_state = 1;
                    counter = 0;
                }
            } else if (controller->buttons_state & SNES_BUTTON_RIGHT) {
                if (initial_position < 5) {
                    ssd1306_SetCursor(46 + (initial_position * 7), 40);
                    ssd1306_WriteString(
                            &player_initials_buffer[initial_position],
                            Font_7x10, White);
                    initial_position++;
                    blink_state = 1;
                    counter = 0;
                }
            } else if (controller->buttons_state & SNES_BUTTON_B) {
                // save string
                player_initials_buffer[6] = '\0';
                printf("%s\n", player_initials_buffer);
                is_done = 1;
            }
        }

        if (blink_state && !counter) {
            ssd1306_SetCursor(46 + (initial_position * 7), 40);
            ssd1306_WriteString(&player_initials_buffer[initial_position],
                    Font_7x10, White);
            blink_state = 0;
            counter = 3;
            ssd1306_UpdateScreen();
        } else if (!blink_state && !counter) {
            ssd1306_SetCursor(46 + (initial_position * 7), 40);
            ssd1306_WriteString(" ", Font_7x10, White);
            blink_state = 1;
            counter = 3;
            ssd1306_UpdateScreen();
        }
        counter--;
        HAL_Delay(BLINK_DELAY * 3);
    }

}
