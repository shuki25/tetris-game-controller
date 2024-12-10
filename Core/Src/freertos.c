/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = { .name = "defaultTask", .stack_size = 512 * 4, .priority =
        (osPriority_t) osPriorityNormal, };
/* Definitions for heartbeatTask */
osThreadId_t heartbeatTaskHandle;
const osThreadAttr_t heartbeatTask_attributes = { .name = "heartbeatTask", .stack_size = 128 * 4, .priority =
        (osPriority_t) osPriorityHigh, };
/* Definitions for Rj45LEDTask */
osThreadId_t Rj45LEDTaskHandle;
const osThreadAttr_t Rj45LEDTask_attributes = { .name = "Rj45LEDTask", .stack_size = 128 * 4, .priority =
        (osPriority_t) osPriorityHigh1, };

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartHeartbeatTask(void *argument);
void StartRj45LED(void *argument);

extern void MX_USB_HOST_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
 * @brief  FreeRTOS initialization
 * @param  None
 * @retval None
 */
void MX_FREERTOS_Init(void) {
    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* USER CODE BEGIN RTOS_MUTEX */
    /* add mutexes, ... */
    /* USER CODE END RTOS_MUTEX */

    /* USER CODE BEGIN RTOS_SEMAPHORES */
    /* add semaphores, ... */
    /* USER CODE END RTOS_SEMAPHORES */

    /* USER CODE BEGIN RTOS_TIMERS */
    /* start timers, add new ones, ... */
    /* USER CODE END RTOS_TIMERS */

    /* USER CODE BEGIN RTOS_QUEUES */
    /* add queues, ... */
    /* USER CODE END RTOS_QUEUES */

    /* Create the thread(s) */
    /* creation of defaultTask */
    defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

    /* creation of heartbeatTask */
    heartbeatTaskHandle = osThreadNew(StartHeartbeatTask, NULL, &heartbeatTask_attributes);

    /* creation of Rj45LEDTask */
    Rj45LEDTaskHandle = osThreadNew(StartRj45LED, NULL, &Rj45LEDTask_attributes);

    /* USER CODE BEGIN RTOS_THREADS */
    /* add threads, ... */
    /* USER CODE END RTOS_THREADS */

    /* USER CODE BEGIN RTOS_EVENTS */
    /* add events, ... */
    /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument) {
    /* init code for USB_HOST */
    MX_USB_HOST_Init();
    /* USER CODE BEGIN StartDefaultTask */
    game_init();
    /* Infinite loop */
    for (;;) {
        game_loop();
    }
    /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartHeartbeatTask */
/**
 * @brief Function implementing the heartbeatTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartHeartbeatTask */
void StartHeartbeatTask(void *argument) {
    /* USER CODE BEGIN StartHeartbeatTask */
    /* Infinite loop */
    for (;;) {
        // TODO: Toggle heartbeat LED
        osDelay(1);
    }
    /* USER CODE END StartHeartbeatTask */
}

/* USER CODE BEGIN Header_StartRj45LED */
/**
 * @brief Function implementing the Rj45LEDTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartRj45LED */
void StartRj45LED(void *argument) {
    /* USER CODE BEGIN StartRj45LED */
    /* Infinite loop */
    for (;;) {
        // TODO: Implement RJ45 LED pattern based on I2C data activity
        osDelay(1);
    }
    /* USER CODE END StartRj45LED */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

