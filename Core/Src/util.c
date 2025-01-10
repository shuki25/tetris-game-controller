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
    uint32_t start = TIM2->CNT;
    uint32_t end = start + us;
    uint32_t rollover = 0xFFFFFFFF;
    uint8_t done = 0;

    if (start > end)
        end = rollover - start + end; // It will rollover, so we need to adjust the end time

    while (!done) {
        if (TIM2->CNT >= end && start < end) { // Normal case
            done = 1;
        } else if (TIM2->CNT < start && TIM2->CNT >= end) { // Rollover case
            done = 1;
        }
    }
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
