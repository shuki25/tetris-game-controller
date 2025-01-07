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

#include "snes_controller.h"

/**
 * @brief  Initialize the SNES controller struct
 * @param  None
 * @retval None
 */
snes_controller_status_t snes_controller_init(snes_controller_t *controller, GPIO_TypeDef *latch_port, uint16_t latch_pin,
        GPIO_TypeDef *clock_port, uint16_t clock_pin, GPIO_TypeDef *data_port, uint16_t data_pin,
        uint8_t read_rate) {
    // TODO: Initialize SNES controller struct

    return SNES_CONTROLLER_OK;
}

/**
 * @brief  Latch the SNES controller
 * @param  None
 * @retval None
 */
snes_controller_status_t snes_controller_latch(snes_controller_t *controller) {
    // TODO: Latch the SNES controller

    return SNES_CONTROLLER_OK;
}

/**
 * @brief  Clock the SNES controller
 * @param  None
 * @retval None
 */
snes_controller_status_t snes_controller_clock(snes_controller_t *controller) {
    // TODO: Clock the SNES controller

    return SNES_CONTROLLER_OK;
}

/**
 * @brief  Read the SNES controller
 * @param  None
 * @retval None
 */
snes_controller_status_t snes_controller_read(snes_controller_t *controller) {
    // TODO: Read the SNES controller

    return SNES_CONTROLLER_OK;
}
