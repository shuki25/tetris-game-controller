/*
 * i2c_slave.h
 *
 *  Created on: Feb 22, 2025
 *      Author: Parker
 */

#ifndef INC_I2C_SLAVE_H_
#define INC_I2C_SLAVE_H_

#include "main.h"
#include "game_stats.h"

#define I2C_BUFFER_SIZE (0x3F)
#define REGISTERS_SIZE (0x53)


typedef enum {
    I2C_SLAVE_STATE_IDLE,
    I2C_SLAVE_STATE_RECEIVE,
    I2C_SLAVE_STATE_TRANSMIT,
    I2C_SLAVE_STATE_ERROR,
    I2C_SLAVE_STATE_BUSY,
    I2C_SLAVE_NUM_STATES
} i2c_slave_state_t;

typedef enum {
    I2C_SLAVE_OK, I2C_SLAVE_ERROR
} i2c_slave_status_t;

typedef struct {
    I2C_HandleTypeDef *hi2c;
    i2c_slave_state_t state;
    uint8_t *rx_buffer;
    uint8_t *tx_buffer;
    uint8_t rx_buffer_size;
    uint8_t tx_buffer_size;
    uint8_t rx_index;
    uint8_t tx_index;
    uint8_t address;
    uint8_t timeout;
    uint32_t prev_tick;
} i2c_slave_t;

void initialize_register();
void update_register(game_score_t *high_scores[], game_score_t *current_score, uint32_t game_elapsed_time, uint8_t clock_sync_flag);
uint32_t get_register_command();
void clear_register_command();

#endif /* INC_I2C_SLAVE_H_ */
