/**
 ******************************************************************************
 * @file           : led_indicator.c
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

#include "led_indicator.h"
#include <string.h>
#include "ui.h"

#define LED_INDICATOR_TIMER_FREQUENCY (1000000) // Timer frequency = 1 MHz

/**
 * @brief Convert HAL timer frequency to ms
 * @param delay: Delay in ms
 * @return Delay in ms
 */
uint32_t led_timer_frequency_to_ms(uint32_t delay) {
    return (delay * LED_INDICATOR_TIMER_FREQUENCY) / 1000;
}

/**
 * @brief Initialize the LED
 * @param led: Pointer to the LED structure
 * @param port: GPIO port
 * @param pin: GPIO pin
 * @param delay: Delay in ms
 * @return LED_OK if successful, LED_ERROR if not
 */
led_status_t led_init(led_indicator_t *led, GPIO_TypeDef *port, uint16_t pin, TIM_HandleTypeDef *htim_delay) {
    memset(led, 0, sizeof(led_indicator_t));
    led->port = port;
    led->pin = pin;
    led->uses_pull_up = 1;
    led->htim_delay = htim_delay;
    led->uses_pwm = 0;
    led->pwm_max_duty = LED_MAX_DUTY;
    led->mode = LED_OFF;
    led->blink_on_delay = led_timer_frequency_to_ms(500);
    led->blink_off_delay = led_timer_frequency_to_ms(500);
    led->delay = led_timer_frequency_to_ms(500);
    led->active = 0;
    led->start_time = 0;
    led->end_time = 0;
    return LED_OK;
}

/**
 * @brief Initialize the LED with PWM
 * @param led: Pointer to the LED structure
 * @param hitm: Timer handle
 * @param channel: Timer channel
 * @param delay: Delay in ms
 * @return LED_OK if successful, LED_ERROR if not
 */
led_status_t led_init_pwm(led_indicator_t *led, TIM_HandleTypeDef *htim_pwm, uint32_t channel,
        TIM_HandleTypeDef *htim_delay) {
    memset(led, 0, sizeof(led_indicator_t));
    led->htim_pwm = htim_pwm;
    led->channel = channel;
    led->uses_pwm = 1;
    led->htim_delay = htim_delay;
    led->pwm_max_duty = LED_MAX_DUTY;
    led->mode = LED_OFF;
    led->blink_on_delay = led_timer_frequency_to_ms(500);
    led->blink_off_delay = led_timer_frequency_to_ms(500);
    led->delay = led_timer_frequency_to_ms(500);
    led->active = 0;
    led->start_time = 0;
    led->end_time = 0;
    return LED_OK;
}

/**
 * @brief Check if the LED time has elapsed with rollover handling
 * @param led: Pointer to the LED structure
 * @return 1 if time has expired, 0 if not
 */
uint8_t led_time_expired(led_indicator_t *led) {
    uint32_t current_time = __HAL_TIM_GET_COUNTER(led->htim_delay);
    if (led->start_time <= led->end_time) {
        // Normal case: no rollover
        if (current_time >= led->end_time) {
            return 1;
        } else if (!(current_time >= led->start_time && current_time < led->end_time)) {
            // Reset the LED state if the current time is outside the start and end time
            return 1;
        }
    } else {
        // Rollover case: end_time < start_time
        if (current_time >= led->end_time && current_time < led->start_time) {
            return 1;
        } else if (current_time > led->end_time && current_time < led->start_time) {
            // Reset the LED state if the current time is outside the start and end time
            return 1;
        }
    }
    return 0;
}

/**
 * @brief Set the state of the LED
 * @param led: Pointer to the LED structure
 * @param state: State to set (LED_ON or LED_OFF)
 */
void led_set_state(led_indicator_t *led, led_state_t state) {
    uint8_t state_val = (state == LED_POWER_ON ? 1 : 0);

    if (led->uses_pwm) {
        if (state == LED_POWER_ON) {
            __HAL_TIM_SET_COMPARE(led->htim_pwm, led->channel, led->pwm_max_duty);
        } else {
            __HAL_TIM_SET_COMPARE(led->htim_pwm, led->channel, 0);
        }
    } else {
        if (led->uses_pull_up) {
            state_val = !state_val;
        }
        HAL_GPIO_WritePin(led->port, led->pin, state_val ? GPIO_PIN_SET : GPIO_PIN_RESET);
    }
}

/**
 * @brief Set the mode of the LED
 * @param led: Pointer to the LED structure
 * @param mode: Mode to set
 * @return LED_OK if successful, LED_ERROR if not
 */
led_status_t led_set_mode(led_indicator_t *led, led_mode_t mode) {
    if (mode < LED_STEADY || mode >= LED_COUNT) {
        return LED_ERROR;
    }
    if (led->uses_pwm == 0 && (mode == LED_N_FADE || mode == LED_FADE_CONTINUOUS)) {
        return LED_MODE_NOT_SUPPORTED;
    }
    led->mode = mode;

    switch (mode) {
    case LED_STEADY:
        led->blink_counter = 0;
        led->mode_changed = 1;
        break;
    case LED_BLINK_CONTINUOUS:
    case LED_N_BLINK:
        led->blink_counter = 0;
        led->blink_state = 0;
        led->blink_running = 0;
        led->mode_changed = 1;
        break;
    case LED_FADE_CONTINUOUS:
    case LED_N_FADE:
        led->blink_counter = 0;
        led->blink_state = 0;
        led->blink_running = 0;
        led->mode_changed = 1;
        break;

    default:
        break;
    }
    return LED_OK;
}

/**
 * @brief Calculate the PWM duty cycle
 * @param led: Pointer to the LED structure
 * @return Duty cycle value
 */
uint32_t led_calculate_duty(led_indicator_t *led) {
    uint32_t auto_reload_value = __HAL_TIM_GET_AUTORELOAD(led->htim_delay); // Max counter value
    uint32_t current_tick = __HAL_TIM_GET_COUNTER(led->htim_delay);        // Current timer tick
    uint32_t duty = 0;
    uint16_t delay = 0;
    uint16_t current_tick_diff = 0;

    if (led->start_time <= led->end_time) {
        // Normal case: no rollover
        if (current_tick >= led->start_time && current_tick < led->end_time) {
            delay = led->end_time - led->start_time;
//            duty = ((led->end_time - current_tick) * 100) / delay;
            duty = ((current_tick - led->start_time) * 100) / delay;
        }
    } else {
        // Rollover case: end_time < start_time
        if (current_tick >= led->start_time || current_tick < led->end_time) {
            delay = auto_reload_value - led->start_time + led->end_time;
            if (current_tick >= led->start_time) {
                current_tick_diff = current_tick - led->start_time;
            } else {
                current_tick_diff = auto_reload_value - led->start_time + current_tick;
            }
            duty = (current_tick_diff * 100) / delay;
        }
    }

    if ((led->fade_direction == LED_DUTY_INCREASING && !led->uses_pull_up)
            || (led->fade_direction == LED_DUTY_DECREASING && led->uses_pull_up)) {
        duty = 100 - duty;
    }

    return duty * led->pwm_max_duty / 100;
}

/**
 * @brief Set the maximum PWM duty cycle
 * @param led: Pointer to the LED structure
 * @param max_duty: Maximum duty cycle in percentage (0-100)
 * @return LED_OK if successful, LED_ERROR if not
 */
led_status_t led_set_pwm_max_duty(led_indicator_t *led, uint32_t max_duty) {
    if (led->uses_pwm == 0) {
        return LED_MODE_NOT_SUPPORTED;
    }
    uint32_t auto_reload_value = __HAL_TIM_GET_AUTORELOAD(led->htim_pwm);
    if (max_duty == 0) {
        return LED_ERROR;
    }

    led->pwm_max_duty = (max_duty * auto_reload_value) / 100;

    return LED_OK;
}

/**
 * @brief Set the delay for the LED
 * @param led: Pointer to the LED structure
 * @param delay: Delay in ms
 * @return LED_OK if successful, LED_ERROR if not
 */
led_status_t led_set_delay(led_indicator_t *led, uint16_t delay) {
    led->delay = led_timer_frequency_to_ms(delay);
    return LED_OK;
}

/**
 * @brief Set the blink delay for the LED
 * @param led: Pointer to the LED structure
 * @param on_delay: On delay in ms
 * @param off_delay: Off delay in ms
 * @return LED_OK if successful, LED_ERROR if not
 */
led_status_t led_set_blink_delay(led_indicator_t *led, uint16_t on_delay, uint16_t off_delay) {
    led->blink_on_delay = led_timer_frequency_to_ms(off_delay);
    led->blink_off_delay = led_timer_frequency_to_ms(off_delay);
    return LED_OK;
}

/**
 * @brief Set the number of blinks
 * @param led: Pointer to the LED structure
 * @param n: Number of blinks
 * @return LED_OK if successful, LED_ERROR if not
 */
led_status_t led_set_n_blinks(led_indicator_t *led, uint16_t n) {
    led->blink_counter = n;
    return LED_OK;
}

/**
 * @brief Enter sleep mode
 * @param led: Pointer to the LED structure
 * @return LED_OK if successful, LED_ERROR if not
 */
led_status_t led_enter_sleep(led_indicator_t *led) {
    // Turn off the LED
    led_set_state(led, LED_POWER_OFF);
    return LED_OK;
}

/**
 * @brief Exit sleep mode
 * @param led: Pointer to the LED structure
 * @return LED_OK if successful, LED_ERROR if not
 */
void led_serial_indicator(led_indicator_t *led, led_state_t state) {
    led_set_state(led, state);
}

/**
 * @brief Control the LED
 * @param led: Pointer to the LED structure
 * @return LED_OK if successful, LED_ERROR if not
 */
led_status_t led_indicator(led_indicator_t *led) {

    uint32_t pwm_duty = 0;
//    uint16_t ticks = 0;
//    char buffer[80];

    if (!led->active) {
        return LED_ERROR;
    }

    switch (led->mode) {
    case LED_STEADY:
        if (led->mode_changed) {
            led->mode_changed = 0;
            led_set_state(led, LED_POWER_ON);
        }
        break;

    case LED_BLINK_CONTINUOUS:
    case LED_N_BLINK:
        if (led->mode_changed) {
            led->mode_changed = 0;
            led->blink_state = LED_POWER_OFF;
            led->start_time = __HAL_TIM_GET_COUNTER(led->htim_delay);
            led->end_time = led->start_time; // Set to expire immediately
            led->blink_running = 1;
        }
        if (led->blink_running) {
            if (led_time_expired(led)) {
                led->start_time = __HAL_TIM_GET_COUNTER(led->htim_delay);
                led->end_time = led->start_time
                        + (led->blink_state == LED_POWER_ON ? led->blink_on_delay : led->blink_off_delay);

                if (led->mode == LED_N_BLINK) {
                    if (led->blink_counter == 0 && led->blink_state == LED_POWER_ON) {
                        led->blink_state = LED_POWER_OFF;
                        led_set_state(led, led->blink_state);
                    } else if (led->blink_counter > 0) {
                        led->blink_state = !led->blink_state;
                        if (led->blink_state == LED_POWER_OFF && led->blink_counter > 0) {

                            if (led->blink_counter == 1) {
                                led->blink_counter = 0;
                            } else {
                                led->blink_counter--;
                            }
                        }
                        led_set_state(led, led->blink_state);
                    }
//                    } else if (led->blink_counter == 0 && led->blink_state == LED_POWER_ON) {
//                        led->blink_state = LED_POWER_OFF; // Stop blinking until the counter is reset
//                    }
                } else {
                    led->blink_state = !led->blink_state;
                    led_set_state(led, led->blink_state);
                }
            } else if (led->blink_state == LED_POWER_OFF) {
                // Check if the LED is actually off
                if (led->uses_pwm) {
                    pwm_duty = __HAL_TIM_GET_COMPARE(led->htim_pwm, led->channel);
                    if (pwm_duty != 0) {
                        __HAL_TIM_SET_COMPARE(led->htim_pwm, led->channel, 0);
                    }
                } else {
                    if (led->uses_pull_up) {
                        if (HAL_GPIO_ReadPin(led->port, led->pin) != GPIO_PIN_SET) {
                            HAL_GPIO_WritePin(led->port, led->pin, GPIO_PIN_SET);
                        }
                    } else {
                        if (HAL_GPIO_ReadPin(led->port, led->pin) != GPIO_PIN_RESET) {
                            HAL_GPIO_WritePin(led->port, led->pin, GPIO_PIN_RESET);
                        }
                    }
                }
            }
        }
        break;

    case LED_FADE_CONTINUOUS:
        if (led->mode_changed) {
            led->mode_changed = 0;
            led->blink_state = LED_POWER_OFF;
            led->fade_direction = LED_DUTY_DECREASING;
            led->start_time = __HAL_TIM_GET_COUNTER(led->htim_delay);
            led->end_time = led->start_time; // Set to expire immediately
            led->blink_running = 1;
        }
        if (led->blink_running) {
            if (led_time_expired(led)) {
                if (led->fade_direction == LED_DUTY_DECREASING) {
                    led->fade_direction = LED_DUTY_INCREASING;
//                    print_terminal("OK\tIncreasing Duty\n");
                } else {
                    led->fade_direction = LED_DUTY_DECREASING;
//                    print_terminal("OK\tDecreasing Duty\n");
                }
                led->start_time = __HAL_TIM_GET_COUNTER(led->htim_delay);
                led->end_time = led->start_time
                        + (led->blink_state == LED_POWER_ON ? led->blink_on_delay : led->blink_off_delay);
                if (led->end_time < led->start_time) {
//                    print_terminal("OK\tRollover\n");
                }
            }
            pwm_duty = led_calculate_duty(led);
//            ticks = __HAL_TIM_GET_COUNTER(led->htim_delay);
//            if (ticks > 64000 || ticks < 1000) {
//                snprintf(buffer, sizeof(buffer) - 1, "OK\tTick: %d\tPWM Duty value: %ld\n", ticks, pwm_duty);
//                print_terminal(buffer);
//            }
            __HAL_TIM_SET_COMPARE(led->htim_pwm, led->channel, pwm_duty);

        }
        break;
        break;
    case LED_N_FADE:
        if (led->uses_pwm) {
            __HAL_TIM_SET_COMPARE(led->htim_pwm, led->channel, 1000);
        }
        break;
    case LED_OFF:
        if (led->mode_changed) {
            led->mode_changed = 0;
            led_set_state(led, LED_POWER_OFF);
        }
        break;
    default:
        break;
    }
    return LED_OK;
}
