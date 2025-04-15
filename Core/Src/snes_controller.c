/**
 ******************************************************************************
 * @file           : snes_controller.c
 * @author         : Shawn Peters, Dr. Joshua Butler
 * @date           : Dec 9, 2024
 * @brief          : Driver for the SNES controller
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

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "snes_controller.h"
#include "util.h"
#include "main.h"
#include "itm_debug.h"

/**
 * @brief  Initialize the SNES controller struct
 * @param  None
 * @retval None
 */
snes_controller_status_t snes_controller_init(snes_controller_t *controller, GPIO_TypeDef *latch_port,
        uint16_t latch_pin, GPIO_TypeDef *clock_port, uint16_t clock_pin, GPIO_TypeDef *data_port,
        uint16_t data_pin, uint8_t read_rate) {

    controller->latch_port = latch_port;
    controller->latch_pin = latch_pin;
    controller->clock_port = clock_port;
    controller->clock_pin = clock_pin;
    controller->data_port = data_port;
    controller->data_pin = data_pin;
    controller->buttons_state = 0x0000;
    controller->previous_buttons_state = 0xFFFF;
    controller->read_rate = read_rate;
    controller->delay_length = 1000000 / read_rate;  // frequency to period in microseconds
    controller->time_last_read = TIM2->CNT;
    controller->time_expire = controller->time_last_read + controller->delay_length;
    controller->led_state = 0;

    return SNES_CONTROLLER_OK;
}

/**
 * @brief  Initialize the SNES controller repeater struct
 * @param  repeater: pointer to the repeater struct, target_button: button to repeat, repeat_rate: rate of repeat
 * @retval snes_controller_repeat_status_t
 */
snes_controller_das_status_t snes_controller_delayed_auto_shift_init(snes_controller_das_t *repeater,
        uint16_t target_button) {

    memset(repeater, 0, sizeof(snes_controller_das_t));
    repeater->repeat_status = SNES_CONTROLLER_DAS_INACTIVE;
    repeater->target_button = target_button;
    repeater->repeat_rate = 1000000 / SNES_DAS_DELAY; // 10Hz default repeat rate (Classic NES Tetris);
    repeater->repeat_start_delay = 266228; // 266.228ms (16 frames at 60Hz)
    repeater->repeat_delay = SNES_DAS_DELAY; // 10 Hz

    return SNES_CONTROLLER_OK;
}

/**
 * @brief  Latch the SNES controller
 * @param  None
 * @retval None
 */
snes_controller_status_t snes_controller_latch(snes_controller_t *controller) {

    HAL_GPIO_WritePin(controller->latch_port, controller->latch_pin, GPIO_PIN_SET);
    util_delay_us(12);
    HAL_GPIO_WritePin(controller->latch_port, controller->latch_pin, GPIO_PIN_RESET);

    return SNES_CONTROLLER_OK;
}

/**
 * @brief  Clock the SNES controller
 * @param  None
 * @retval None
 */
snes_controller_status_t snes_controller_clock(snes_controller_t *controller, GPIO_PinState state) {

    HAL_GPIO_WritePin(controller->clock_port, controller->clock_pin, state);
    util_delay_us(5);

    return SNES_CONTROLLER_OK;
}

/**
 * @brief  Read the SNES controller
 * @param  snes_controller_t *controller
 * @retval status of the SNES controller
 */
snes_controller_status_t snes_controller_read(snes_controller_t *controller) {

    // Read rate throttling, if the timer expires, it is ready to read the controller
    if (!util_time_expired(controller->time_last_read, controller->time_expire)) {
        return SNES_CONTROLLER_NOT_READY;
    }

    snes_controller_latch(controller);
    controller->buttons_state = 0x0000;

    controller->time_last_read = TIM2->CNT;
    controller->time_expire = controller->time_last_read + controller->delay_length;
    for (int i = 0; i < 16; i++) {
        snes_controller_clock(controller, GPIO_PIN_RESET);
        controller->buttons_state |= (HAL_GPIO_ReadPin(controller->data_port, controller->data_pin)
                << (15 - i));
        snes_controller_clock(controller, GPIO_PIN_SET);
    }

    // Invert the button state as the controller is active low
    controller->buttons_state = ~controller->buttons_state;

    if (controller->buttons_state == 0xFFFF) {
        return SNES_CONTROLLER_DISCONNECTED;
    }

    if (controller->buttons_state != controller->previous_buttons_state) {
        controller->previous_buttons_state = controller->buttons_state;
        return SNES_CONTROLLER_STATE_CHANGE;
    }

    return SNES_CONTROLLER_NO_STATE_CHANGE;
}

/**
 * @brief  Check the repeat status of the SNES controller
 * @param  snes_controller_repeat_t *repeater, snes_controller_t *controller
 * @retval None
 */
void snes_controller_delayed_auto_shift(snes_controller_das_t *repeater, snes_controller_t *controller) {
    repeater->buttons_state = controller->buttons_state;

    if (repeater->repeat_status == SNES_CONTROLLER_DAS_INACTIVE) {
        if (controller->buttons_state & repeater->target_button
                && !(repeater->previous_buttons_state & repeater->target_button)) {
            repeater->repeat_status = SNES_CONTROLLER_DAS_START;
            repeater->repeat_start_time = TIM2->CNT;
        }
    } else if (repeater->repeat_status == SNES_CONTROLLER_DAS_START) {
        if (util_time_expired_delay(repeater->repeat_start_time, repeater->repeat_start_delay)) {
            repeater->repeat_status = SNES_CONTROLLER_DAS_ACTIVE_IDLE;
            repeater->repeat_start_time = TIM2->CNT;

        }
    } else if (repeater->repeat_status == SNES_CONTROLLER_DAS_ACTIVE_IDLE) {
        // Check if the button is still pressed
        if (!(controller->buttons_state & repeater->target_button)) {
            repeater->repeat_status = SNES_CONTROLLER_DAS_INACTIVE;
        } else {
            // Check if the repeat delay has expired
            if (util_time_expired_delay(repeater->repeat_start_time, repeater->repeat_delay)) {
                repeater->repeat_status = SNES_CONTROLLER_DAS_ACTIVE_ENQUEUE;
            }
        }
    } else if (repeater->repeat_status == SNES_CONTROLLER_DAS_ACTIVE_ENQUEUE) {
        // Check if the button is still pressed
        if (!(controller->buttons_state & repeater->target_button)) {
            repeater->repeat_status = SNES_CONTROLLER_DAS_INACTIVE;
        } else {
            repeater->repeat_status = SNES_CONTROLLER_DAS_ACTIVE_IDLE;
            repeater->repeat_start_time = TIM2->CNT;
        }
    } else {
        // This should never happen
        repeater->repeat_status = SNES_CONTROLLER_DAS_INACTIVE;
    }

    repeater->previous_buttons_state = repeater->buttons_state;
}

/**
 * @brief  Print the SNES controller state (Debugging purposes)
 * @param  snes_controller_t *controller
 * @retval None
 */
void snes_controller_print(snes_controller_t *controller) {
    char output_buffer[80];
    if (controller->buttons_state == 0xFFFF || controller->buttons_state == 0x0000) {
        return;
    }
    util_to_binary16(controller->buttons_state, output_buffer);
    printf("Controller: 0b%s [", output_buffer);
    if (controller->buttons_state & SNES_BUTTON_B) {
        printf("B ");
    }
    if (controller->buttons_state & SNES_BUTTON_Y) {
        printf("Y ");
    }
    if (controller->buttons_state & SNES_BUTTON_SELECT) {
        printf("Select ");
    }
    if (controller->buttons_state & SNES_BUTTON_START) {
        printf("Start ");
    }
    if (controller->buttons_state & SNES_BUTTON_UP) {
        printf("Up ");
    }
    if (controller->buttons_state & SNES_BUTTON_DOWN) {
        printf("Down ");
    }
    if (controller->buttons_state & SNES_BUTTON_LEFT) {
        printf("Left ");
    }
    if (controller->buttons_state & SNES_BUTTON_RIGHT) {
        printf("Right ");
    }
    if (controller->buttons_state & SNES_BUTTON_A) {
        printf("A ");
    }
    if (controller->buttons_state & SNES_BUTTON_X) {
        printf("X ");
    }
    if (controller->buttons_state & SNES_BUTTON_L) {
        printf("L ");
    }
    if (controller->buttons_state & SNES_BUTTON_R) {
        printf("R ");
    }
    printf("]\n");
}
