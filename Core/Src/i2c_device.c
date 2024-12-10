/**
  ******************************************************************************
  * @file           : i2c_device.c
  * @author         : Skylar Aubertin-Farmer, Dr. Joshua Butler
  * @date           : Dec 9, 2024
  * @brief          : I2C device client for Classic Tetris on LED Grid
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

/*******************************************************************************

 * To write the implementation of the i2c_device.c file, you will need to follow
 * a tutorial by Controllers Tech on YouTube. The tutorial is called "STM32 as
 * I2C Slave" and can be found at the following URL:
 *
 * https://www.youtube.com/watch?v=X7WZQka2FHQ
 *
 * The tutorial is also available in written form at the following URL:
 *
 * https://controllerstech.com/stm32-as-i2c-slave-part-1/
 *
 * This file will contain the implementation of the I2C device client for the
 * Classic Tetris controller that responds to I2C requests from the scoreboard
 * controller.
 *
 * For this project, you will use i2c2 as the I2C device as i2c1 is already
 * being used by the other components on the controller board. The I2C device
 * is already configured in the CubeMX project.
 *******************************************************************************
 */


#include "i2c_device.h"

/**
 * @brief  Initialize I2C device
 * @param  i2c device object
 * @retval status
 */
void i2c_device_init(void) {
    // TODO: Initialize I2C device object
}

/**
 * @brief  Read I2C device register
 * @param  i2c device object, register address
 * @retval status
 */
void i2c_device_register_init(void) {
    // TODO: Initialize I2C device register
}

/**
 * @brief  Convert I2C device object to register value
 * @param  i2c device object, register address
 * @retval status
 */
void i2c_device_object2register(void) {
    // TODO: convert I2C device object to register that can be transmitted over I2C
}
