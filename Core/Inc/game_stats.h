/*
 * game_stats.h
 *
 *  Created on: Feb 22, 2025
 *      Author: Parker
 */

#ifndef INC_GAME_STATS_H_
#define INC_GAME_STATS_H_

#define NAME_LENGTH (4)
#define NUM_HIGH_SCORES (5)

typedef struct saved_settings {
    uint8_t grid_size;
    uint8_t brightness;
} saved_settings_t;

typedef struct game_score { // 96 bits
    char name[NAME_LENGTH]; // 32 bits (8 bits per char * 4 chars)
    uint32_t score; // 32 bits
    uint16_t level; //  16 bits
    uint16_t lines; // 16 bits
} game_score_t;

#endif /* INC_GAME_STATS_H_ */
