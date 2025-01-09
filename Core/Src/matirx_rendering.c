/**
  ******************************************************************************
  * @file           : matirx_rendering.c
  * @author         : Yoel Buzgalo, Dr. Joshua Butler
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

#include "matrix_rendering.h"
#include "ws2812.h"
#include "cmsis_os.h"
extern led_t led;
extern TIM_HandleTypeDef htim3;
WS2812_error_t led_error;

/**
 * @brief  Initialize WS2812 LED matrix
 * @param  None
 * @retval None
 */
void matrix_rendering_init() {
    // TODO: Initialize WS2812 LED matrix
    led_error = WS2812_init(&led, &htim3, TIM_CHANNEL_1, htim3.Init.Period, 16, 0);
    if (led_error != WS2812_OK) {
        Error_Handler();
    }

    led.data_sent_flag = 1;

    WS2812_fill(&led, 0, 0, 32);
//    WS2812_set_brightness(&led, 5);
    WS2812_send(&led);
    osDelay(10);
}

/**
 * @brief  Render final matrix to WS2812 LED matrix
 * @param  None
 * @retval None
 */
void matrix_rendering_render(void) {
    // TODO: Send final matrix to WS2812 LED matrix
}

/**
 * @brief  Clear WS2812 LED matrix
 * @param  None
 * @retval None
 */
void matrix_rendering_clear(void) {
    // TODO: Clear WS2812 LED matrix
}

/**
 * @brief  Show next tetrimino on WS2812 LED matrix
 * @param  tetrimino identifier
 * @retval None
 */
void matrix_rendering_show_next_tetrimino(void) {
    // TODO: Show next tetrimino on WS2812 LED matrix
}
