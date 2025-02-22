/*
 * scoreboard.h
 *
 *  Created on: Fev 22, 2025
 *      Author: Parker
 */

#ifndef INC_SCOREBOARD_H_
#define INC_SCOREBOARD_H_

// Bit Definitions for the console_info
#define CONSOLE_SIGNATURE   (0b11000000) // Fixed signature bits
#define CONSOLE_IDENTIFIER  (0b00000111) // Between 1 and 5 (I2C addresses 0x10 - 0x14)
#define CONSOLE_CLOCK_SYNC  (0b00001000) // 0 = no, 1 = yes
#define CONSOLE_CLOCK_SHIFT (3)

// 32-Bit Definitions for the date/time
// 31    26   22    17    12     6      0
// +------+----+-----+-----+------+------+
// |YYYYYY|MMMM|DDDDD|HHHHH|MMMMMM|SSSSSS|
// +------+----+-----+-----+------+------+
#define YEAR_MASK   (0b00111111 << 26)
#define YEAR_SHIFT  (26)
#define MONTH_MASK  (0b00001111 << 22)
#define MONTH_SHIFT (22)
#define DAY_MASK    (0b00011111 << 17)
#define DAY_SHIFT   (17)
#define HOUR_MASK   (0b00011111 << 12)
#define HOUR_SHIFT  (12)
#define MINUTE_MASK (0b00111111 << 6)
#define MINUTE_SHIFT (6)
#define SECOND_MASK (0b00111111)
#define SECOND_SHIFT (0)

// 32-Bit Definitions for the command
// 31    26  23     16        8        0
// +------+---+-------+--------+--------+
// |000000|CMD|RESERVE|  PARAM2|  PARAM1|
// +------+---+-------+--------+--------+

#define I2C_CMD_SET_SPEED       (0b001 << 23)
#define I2C_CMD_SET_LEVEL       (0b010 << 23)
#define I2C_CMD_SET_POISON      (0b011 << 23)
#define I2C_CMD_PREPARE_GAME    (0b100 << 23)
#define I2C_CMD_START_GAME      (0b101 << 23)
#define I2C_CMD_PAUSE_GAME      (0b110 << 23)
#define I2C_CMD_END_GAME        (0b111 << 23)
#define I2C_CMD_MASK            (0b111 << 23)
#define PARAM1_MASK             (0b11111111)
#define PARAM1_SHIFT            (0)
#define PARAM2_MASK             (0b11111111 << 8)
#define PARAM2_SHIFT            (8)

typedef struct {                                // Register Map
    uint8_t console_info;                       // 0x00 (8 bits)
    game_score_t *high_scores[NUM_HIGH_SCORES]; // 0x01 (480 bits: 96 bits per score with 5 highscores)
    game_score_t *current_score;                // 0x3D (96 bits)
    uint16_t playing_time;                      // 0x49 (16 bits)
    uint32_t date_time;                         // 0x4B (32 bits)
    uint32_t command;                           // 0x4F (32 bits)
} scoreboard_t;

#endif /* INC_SCOREBOARD_H_ */
