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

// TODO: Typedef for controller status in enum (e.g. SNES_CONTROLLER_OK, SNES_CONTROLLER_ERROR)
typedef enum {
	SNES_CONTROLLER_OK = 0,
	SNES_CONTROLLER_ERROR
} snes_controller_status_t;

// TODO: Defines for shift register bit order and button mapping (B, Y, Select, Start, Up, Down, Left, Right, A, X, L, R)
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

// TODO: Typedef for SNES controller struct (e.g. snes_controller_t) for GPIO pins assignment for latch, clock, data, and button state
typedef struct {
	GPIO_TypeDef* latch_port;
	uint16_t latch_pin;
	GPIO_TypeDef* clock_port;
	uint16_t clock_pin;
	GPIO_TypeDef* data_port;
	uint16_t data_pin;
	uint16_t buttons_state;
	uint16_t previous_buttons_state;
	uint8_t read_rate; // in Hz
	uint32_t last_read;
	uint32_t delay_expire;
} snes_controller_t;

// TODO: Function prototypes for SNES controller functions (e.g. snes_controller_init, snes_controller_latch, snes_controller_clock, snes_controller_read)
void snes_controller_init(snes_controller_t *controller, GPIO_TypeDef *latch_port, uint16_t latch_pin,
        GPIO_TypeDef *clock_port, uint16_t clock_pin, GPIO_TypeDef *data_port, uint16_t data_pin,
        uint8_t read_rate);
void snes_controller_latch(snes_controller_t* controller);
void snes_controller_clock(snes_controller_t* controller);
uint16_t snes_controller_read(snes_controller_t* controller);

#endif /* INC_SNES_CONTROLLER_H_ */
