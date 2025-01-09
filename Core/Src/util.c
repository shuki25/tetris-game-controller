/**
 ******************************************************************************
 * @file           : util.c
 * @author         : josh
 * @date           : Jan 7, 2025
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

#include <stdint.h>
#include "main.h"
#include "util.h"

void util_delay_us(uint32_t us) {
    uint32_t start = DWT->CYCCNT;
    uint32_t delay = us * (HAL_RCC_GetHCLKFreq() / 1000000);
    while (DWT->CYCCNT - start < delay)
        ;
}

void util_to_binary32(uint32_t num, char *binary) {
    for (int i = 0; i < 32; i++) {
        binary[i] = (num & (1 << (31 - i))) ? '1' : '0';
    }
    binary[32] = '\0';
}

void util_to_binary16(uint16_t num, char *binary) {
    for (int i = 0; i < 16; i++) {
        binary[i] = (num & (1 << (15 - i))) ? '1' : '0';
    }
    binary[16] = '\0';
}

void util_to_binary8(uint8_t num, char *binary) {
    for (int i = 0; i < 8; i++) {
        binary[i] = (num & (1 << (7 - i))) ? '1' : '0';
    }
    binary[8] = '\0';
}
