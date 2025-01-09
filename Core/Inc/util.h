/**
 ******************************************************************************
 * @file           : util.h
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

#ifndef INC_UTIL_H_
#define INC_UTIL_H_

void util_delay_us(uint32_t us);
void util_to_binary32(uint32_t num, char *binary);
void util_to_binary16(uint16_t num, char *binary);
void util_to_binary8(uint8_t num, char *binary);

#endif /* INC_UTIL_H_ */
