/*
 * high_score_handler.c
 *
 *  Created on: Apr 12, 2025
 *      Author: yoel
 */

#include "high_score_handler.h"
#include "ssd1306.h"
#include "snes_controller.h"
#include "eeprom.h"
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

uint8_t get_high_score_index(game_t *game, game_high_score_t **high_score){
    for(int i = 0; i < EEPROM_NUM_HIGH_SCORES; i++){
        if (game->score > high_score[i]){
            return i;
        }
    }
}
