/**
 ******************************************************************************
 * @file           : snes_controller.h
 * @author         : Shawn Peters, Dr. Joshua Butler
 * @date           : Dec 9, 2024
 * @brief          :
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

#ifndef INC_SNES_CONTROLLER_H_
#define INC_SNES_CONTROLLER_H_

#include "main.h"

typedef enum {
    SNES_CONTROLLER_OK = 0,
    SNES_CONTROLLER_ERROR,
    SNES_CONTROLLER_STATE_CHANGE,
    SNES_CONTROLLER_NO_STATE_CHANGE,
    SNES_CONTROLLER_NOT_READY,
    SNES_CONTROLLER_DISCONNECTED
} snes_controller_status_t;

// Delayed Auto Shift (DAS) Status
typedef enum {
    SNES_CONTROLLER_DAS_OK = 0,
    SNES_CONTROLLER_DAS_INACTIVE,
    SNES_CONTROLLER_DAS_START,
    SNES_CONTROLLER_DAS_ACTIVE_IDLE,
    SNES_CONTROLLER_DAS_ACTIVE_ENQUEUE,
} snes_controller_das_status_t;

// Defines for shift register bit order and button mapping (B, Y, Select, Start, Up, Down, Left, Right, A, X, L, R)
#define SNES_BUTTON_B	(1 << 15)
#define SNES_BUTTON_Y	(1 << 14)
#define SNES_BUTTON_SELECT	(1 << 13)
#define SNES_BUTTON_START	(1 << 12)
#define SNES_BUTTON_UP	(1 << 11)
#define SNES_BUTTON_DOWN	(1 << 10)
#define SNES_BUTTON_LEFT	(1 << 9)
#define SNES_BUTTON_RIGHT	(1 << 8)
#define SNES_BUTTON_A	(1 << 7)
#define SNES_BUTTON_X	(1 << 6)
#define SNES_BUTTON_L	(1 << 5)
#define SNES_BUTTON_R	(1 << 4)

#define SNES_DAS_DELAY (100000) // 10Hz default repeat rate (Classic NES Tetris)

typedef struct {
    GPIO_TypeDef *latch_port;
    uint16_t latch_pin;
    GPIO_TypeDef *clock_port;
    uint16_t clock_pin;
    GPIO_TypeDef *data_port;
    uint16_t data_pin;
    uint16_t buttons_state;
    uint16_t previous_buttons_state;
    uint8_t read_rate; // in Hz
    uint32_t time_last_read;
    uint32_t delay_length;
    uint32_t time_expire;
    uint8_t led_state;
} snes_controller_t;

typedef struct {
    snes_controller_das_status_t repeat_status; // active or inactive
    uint16_t target_button; // which button state to determine repeat
    uint16_t buttons_state;
    uint16_t previous_buttons_state;
    uint8_t repeat_rate; // in Hz
    uint32_t repeat_start_time;
    uint32_t repeat_start_delay; // delay before repeat starts (in microseconds)
    uint32_t repeat_delay; // in microseconds (based on repeat rate)
} snes_controller_das_t;

snes_controller_status_t snes_controller_init(snes_controller_t *controller, GPIO_TypeDef *latch_port,
        uint16_t latch_pin, GPIO_TypeDef *clock_port, uint16_t clock_pin, GPIO_TypeDef *data_port,
        uint16_t data_pin, uint8_t read_rate);
snes_controller_das_status_t snes_controller_delayed_auto_shift_init(snes_controller_das_t *repeater,
        uint16_t target_button);
snes_controller_status_t snes_controller_latch(snes_controller_t *controller);
snes_controller_status_t snes_controller_clock(snes_controller_t *controller, GPIO_PinState state);
snes_controller_status_t snes_controller_read(snes_controller_t *controller);
void snes_controller_delayed_auto_shift(snes_controller_das_t *repeater, snes_controller_t *controller);
void snes_controller_print(snes_controller_t *controller);

#endif /* INC_SNES_CONTROLLER_H_ */
