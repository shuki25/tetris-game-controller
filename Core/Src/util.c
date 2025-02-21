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

/**
 * @brief  Check if time has expired (rollover is handled)
 * @param  start: start time
 * @param  delay: delay time
 * @retval 1 if expired, 0 if not expired
 */
uint8_t util_time_expired_delay(uint32_t start, uint32_t delay) {
    uint32_t end = start + delay;
    uint32_t rollover = 0xFFFFFFFF;

    if (start > end)
        end = rollover - start + end; // It will rollover, so we need to adjust the end time

    if (TIM2->CNT >= end && start < end) { // Normal case
        return 1;
    } else if (TIM2->CNT < start && TIM2->CNT >= end) { // Rollover case
        return 1;
    }

    return 0;
}

/**
 * @brief  Check if time has expired (rollover is handled)
 * @param  start: start time
 * @param  end: end time
 * @retval 1 if expired, 0 if not expired
 */
uint8_t util_time_expired(uint32_t start, uint32_t end) {
    uint32_t rollover = 0xFFFFFFFF;

    if (start > end)
        end = rollover - start + end; // It will rollover, so we need to adjust the end time

    if (TIM2->CNT >= end && start < end) { // Normal case
        return 1;
    } else if (TIM2->CNT < start && TIM2->CNT >= end) { // Rollover case
        return 1;
    }

    return 0;
}

/**
 * @brief  Get time difference in microseconds (rollover is handled)
 * @param  start: start time
 * @param  end: end time
 * @retval time difference in microseconds
 */
uint32_t util_time_diff_us(uint32_t start, uint32_t end) {
    uint32_t rollover = 0xFFFFFFFF;

    if (start > end)
        end = rollover - start + end; // It will rollover, so we need to adjust the end time

    if (TIM2->CNT >= end && start < end) { // Normal case
        return end - start;
    } else if (TIM2->CNT < start && TIM2->CNT >= end) { // Rollover case
        return (rollover - start) + end;
    }

    return 0;
}

/**
 * @brief  Delay in microseconds
 * @param  us: microseconds
 * @retval None
 */
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

/**
 * @brief  Convert 32-bit number to binary string
 * @param  num: 32-bit number
 * @param  binary: binary string
 * @retval None
 */
void util_to_binary32(uint32_t num, char *binary) {
    for (int i = 0; i < 32; i++) {
        binary[i] = (num & (1 << (31 - i))) ? '1' : '0';
    }
    binary[32] = '\0';
}

/**
 * @brief  Convert 16-bit number to binary string
 * @param  num: 16-bit number
 * @param  binary: binary string
 * @retval None
 */
void util_to_binary16(uint16_t num, char *binary) {
    for (int i = 0; i < 16; i++) {
        binary[i] = (num & (1 << (15 - i))) ? '1' : '0';
    }
    binary[16] = '\0';
}

/**
 * @brief  Convert 8-bit number to binary string
 * @param  num: 8-bit number
 * @param  binary: binary string
 * @retval None
 */
void util_to_binary8(uint8_t num, char *binary) {
    for (int i = 0; i < 8; i++) {
        binary[i] = (num & (1 << (7 - i))) ? '1' : '0';
    }
    binary[8] = '\0';
}

/**
 * @brief  Count number of bits set in a number
 * @param  num: number
 * @retval number of bits set
 */
uint8_t util_bit_count(uint32_t num) {
    uint8_t count = 0;
    while (num) {
        count += num & 1;
        num >>= 1;
    }
    return count;
}
