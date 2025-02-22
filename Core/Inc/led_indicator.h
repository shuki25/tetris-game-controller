/**
 ******************************************************************************
 * @file           : led_indicator.h
 * @author         : josh
 * @date           : Dec 14, 2024
 * @brief          : LED Indicator functions to control the LEDs on the board
 ******************************************************************************
 * @attention
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

#ifndef INC_LED_INDICATOR_H_
#define INC_LED_INDICATOR_H_

#include "main.h"

#define LED_MAX_DUTY 1000

typedef enum {
    LED_OK = 0, LED_ERROR, LED_MODE_NOT_SUPPORTED
} led_status_t;

typedef enum {
    LED_STEADY = 0, LED_BLINK_CONTINUOUS, LED_N_BLINK, LED_FADE_CONTINUOUS, LED_N_FADE, LED_OFF, LED_COUNT
} led_mode_t;

typedef enum {
    LED_POWER_OFF = 0, LED_POWER_ON
} led_state_t;

typedef enum {
    LED_DUTY_INCREASING = 0, LED_DUTY_DECREASING
} led_fade_t;

typedef struct {
    TIM_HandleTypeDef *htim_pwm;
    GPIO_TypeDef *port;
    uint16_t pin;
    uint32_t channel;
    TIM_HandleTypeDef *htim_delay;
    uint8_t uses_pull_up; // 0 = no, 1 = yes; if yes, low to turn on LED
    uint8_t uses_pwm;
    uint32_t pwm_max_duty;
    uint8_t mode_changed;
    led_mode_t mode;
    uint8_t active;
    uint32_t delay;
    led_state_t blink_state; // 0 = off, 1 = on
    uint8_t blink_running; // 0 = off, 1 = on
    uint32_t blink_on_delay;
    uint32_t blink_off_delay;
    uint16_t blink_counter;
    led_fade_t fade_direction;
    uint32_t start_time;
    uint32_t end_time;
} led_indicator_t;

// Function prototypes
led_status_t led_init(led_indicator_t *led, GPIO_TypeDef *port, uint16_t pin, TIM_HandleTypeDef *htim_delay);
led_status_t led_init_pwm(led_indicator_t *led, TIM_HandleTypeDef *htim, uint32_t channel,
        TIM_HandleTypeDef *htim_delay);
led_status_t led_set_mode(led_indicator_t *led, led_mode_t mode);
led_status_t led_set_pwm_max_duty(led_indicator_t *led, uint32_t max_duty);
led_status_t led_set_delay(led_indicator_t *led, uint16_t delay);
led_status_t led_set_blink_delay(led_indicator_t *led, uint16_t on_delay, uint16_t off_delay);
led_status_t led_set_n_blinks(led_indicator_t *led, uint16_t n);
led_status_t led_enter_sleep(led_indicator_t *led);
void led_serial_indicator(led_indicator_t *led, led_state_t state);
led_status_t led_indicator(led_indicator_t *led);

#endif /* INC_LED_INDICATOR_H_ */
