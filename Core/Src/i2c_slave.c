/*
 * i2c_slave.c
 *
 *  Created on: Feb 22, 2025
 *      Author: Parker
 */

#include "main.h"
#include "i2c_slave.h"
#include "scoreboard.h"
#include <string.h>
#include "led_indicator.h"

volatile uint8_t i2c_rx_buffer[I2C_BUFFER_SIZE];
volatile uint8_t i2c_rx_index = 0;
volatile uint8_t i2c_tx_index = 0;
volatile uint8_t bytes_received = 0;
volatile uint8_t start_position = 0;
volatile scoreboard_t i2c_register_struct;
volatile uint8_t i2c_register[REGISTERS_SIZE] = { 0 };

extern I2C_HandleTypeDef hi2c2;
extern RTC_HandleTypeDef hrtc;
extern led_indicator_t scoreboard_led;

void volatile_memcpy(volatile void *dest, void *src, size_t n) {
    for (size_t i = 0; i < n; i++) {
        *((volatile uint8_t*) dest + i) = *((uint8_t*) src + i);
    }
}

void volatile_memset(volatile void *dest, uint8_t value, size_t n) {
    for (size_t i = 0; i < n; i++) {
        *((volatile uint8_t*) dest + i) = value;
    }
}

void initialize_register() {
    i2c_register_struct.console_info = 0;
    i2c_register_struct.playing_time = 0;
    i2c_register_struct.date_time = 0;
    i2c_register_struct.command = 0;
    volatile_memset(i2c_register, 0, REGISTERS_SIZE);
}

void volatile_input_value_to_register(volatile void *val, uint8_t currRegister, size_t size) {
    uint8_t *byte_val = (uint8_t *)val;

    // Put the value into register 8 bits at a time
    for (size_t i = 0; i < size; i++) {
        i2c_register[currRegister++] = byte_val[i];
    }
}

void score_to_register(game_score_t *score, uint8_t currRegister) {
    //// Each score is 96 bits and we have to put it in 8 bits at a time

    // Name
    for (int i = 0; i < NAME_LENGTH; i++) {
        i2c_register[currRegister++] = (uint8_t) score->name[i] & 0xFF;
    }

    // Score, level, and lines
    volatile_input_value_to_register(&score->score, currRegister, sizeof(score->score));
    volatile_input_value_to_register(&score->level, currRegister, sizeof(score->level));
    volatile_input_value_to_register(&score->lines, currRegister, sizeof(score->lines));
}

void struct2register() {
    uint8_t i = 0;

    //// Put data into registers 8 bits at a time using shifting and bit masking
    i2c_register[i++] = i2c_register_struct.console_info;

    // Put in current score
    score_to_register(i2c_register_struct.current_score, i);

    // Put in high score
    for (int j = 0; j < NUM_HIGH_SCORES; j++) {
        score_to_register(i2c_register_struct.high_scores[j], i);
    }

    // Put in misc. data
    volatile_input_value_to_register(&i2c_register_struct.playing_time, i, sizeof(i2c_register_struct.playing_time));
    volatile_input_value_to_register(&i2c_register_struct.date_time, i, sizeof(i2c_register_struct.date_time));
    volatile_input_value_to_register(&i2c_register_struct.command, i, sizeof(i2c_register_struct.command));
}

void update_register(game_score_t *high_scores[], game_score_t *current_score, uint32_t game_elapsed_time,
        uint8_t clock_sync_flag) {

    RTC_TimeTypeDef sTime;
    RTC_DateTypeDef sDate;

    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
    uint8_t console_id = ((I2C2->OAR1 & 0x7E) >> 1) - 15;
    i2c_register_struct.console_info = console_id | CONSOLE_SIGNATURE
            | (clock_sync_flag << CONSOLE_CLOCK_SHIFT);

    i2c_register_struct.current_score = current_score;
    for (int i = 0; i < NUM_HIGH_SCORES; i++) {
        i2c_register_struct.high_scores[i] = high_scores[i];
    }    i2c_register_struct.playing_time = (uint16_t) game_elapsed_time;
    i2c_register_struct.date_time = (sDate.Year << YEAR_SHIFT) | (sDate.Month << MONTH_SHIFT)
            | (sDate.Date << DAY_SHIFT) | (sTime.Hours << HOUR_SHIFT) | (sTime.Minutes << MINUTE_SHIFT)
            | (sTime.Seconds << SECOND_SHIFT);

    struct2register();
}

uint32_t get_register_command() {
    return i2c_register_struct.command;
}

void clear_register_command() {
    i2c_register_struct.command = 0;
}

void process_data() {
    if (bytes_received > 0) {
        if (i2c_rx_buffer[0] == 0x30 && bytes_received == 5) {
            i2c_register_struct.command = (i2c_rx_buffer[1] << 24) | (i2c_rx_buffer[2] << 16)
                    | (i2c_rx_buffer[3] << 8) | i2c_rx_buffer[4];
        }
        bytes_received = 0;
        i2c_rx_index = 0;
        i2c_tx_index = 0;
        struct2register();
    }
}

void HAL_I2C_ListenCpltCallback(I2C_HandleTypeDef *hi2c) {
    HAL_I2C_EnableListen_IT(hi2c);
}

void HAL_I2C_AddrCallback(I2C_HandleTypeDef *hi2c, uint8_t TransferDirection, uint16_t AddrMatchCode) {
    if (TransferDirection == I2C_DIRECTION_RECEIVE) { // Master receiving from slave
        start_position = i2c_rx_buffer[0];
        i2c_rx_buffer[0] = 0;
        i2c_tx_index = 0;
        // TODO: figure this out
        //led_indicator_set_blink(&scoreboard_led, 40, 6);
        HAL_I2C_Slave_Seq_Transmit_IT(hi2c, (uint8_t*) &i2c_register + start_position, 1, I2C_FIRST_FRAME);
    } else if (TransferDirection == I2C_DIRECTION_TRANSMIT) { // Master transmitting to slave
        i2c_rx_index = 0;
        i2c_tx_index = 0;
        volatile_memset((uint8_t*) i2c_rx_buffer, 0, I2C_BUFFER_SIZE);
        HAL_I2C_Slave_Seq_Receive_IT(hi2c, (uint8_t*) &i2c_rx_buffer, 1, I2C_FIRST_FRAME);
    } else {
        Error_Handler();
    }
}

void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c) {
    i2c_rx_index++;
    bytes_received++;
    if (i2c_rx_index < I2C_BUFFER_SIZE) {
        if (i2c_rx_index == I2C_BUFFER_SIZE - 1) {
            HAL_I2C_Slave_Seq_Receive_IT(hi2c, (uint8_t*) &i2c_rx_buffer + i2c_rx_index, 1, I2C_LAST_FRAME);
        } else {
            HAL_I2C_Slave_Seq_Receive_IT(hi2c, (uint8_t*) &i2c_rx_buffer + i2c_rx_index, 1, I2C_NEXT_FRAME);
        }
    }
    if (i2c_rx_index == I2C_BUFFER_SIZE) {
        process_data();
    }
}

void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c) {
    i2c_tx_index++;
    HAL_I2C_Slave_Seq_Transmit_IT(hi2c, (uint8_t*) &i2c_register + start_position + i2c_tx_index, 1,
    I2C_NEXT_FRAME);
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c) {
    uint32_t error = HAL_I2C_GetError(hi2c);
    if (error == HAL_I2C_ERROR_AF) {
        __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_AF);
        if (i2c_tx_index == 0) { // error occured while slave is receiving
            i2c_rx_index = 0;
            process_data();
        } else { // error occured while slave is transmitting
            i2c_tx_index--;
        }
    } else if (error == HAL_I2C_ERROR_BERR) {
        HAL_I2C_DeInit(hi2c);
        HAL_I2C_Init(hi2c);
        volatile_memset((uint8_t*) i2c_rx_buffer, 0, I2C_BUFFER_SIZE);
        bytes_received = 0;
        i2c_rx_index = 0;
        i2c_tx_index = 0;
    }
    HAL_I2C_EnableListen_IT(hi2c);
}
