/*
 * high_score_handler.h
 *
 *  Created on: Apr 12, 2025
 *      Author: yoel
 */

#ifndef INC_HIGH_SCORE_HANDLER_H_
#define INC_HIGH_SCORE_HANDLER_H_

#define BLINK_DELAY (50)

#include "snes_controller.h"
#include "game_loop.h"
#include "eeprom.h"

uint8_t check_high_score(game_t *game, game_high_score_t **high_score);

eeprom_status_t save_initials_to_high_score(eeprom_t *eeprom, char* buffer, game_t *game, game_high_score_t **high_score);

#endif /* INC_HIGH_SCORE_HANDLER_H_ */
