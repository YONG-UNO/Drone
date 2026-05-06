/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
osThreadId GimbalHandle;
osThreadId ShootHandle;
osThreadId AutoAimingHandle;
osThreadId Gimbal_RS05Handle;
osThreadId USBTaskHandle;
osThreadId myTask06Handle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void gimbalControl(void const * argument);
void shootControl(void const * argument);
void autoAiming(void const * argument);
void gimbalControl_RS05(void const * argument);
void USB(void const * argument);
void gc_task(void const * argument);

extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

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
  /* definition and creation of Gimbal */
  osThreadDef(Gimbal, gimbalControl, osPriorityNormal, 0, 256);
  GimbalHandle = osThreadCreate(osThread(Gimbal), NULL);

  /* definition and creation of Shoot */
  osThreadDef(Shoot, shootControl, osPriorityIdle, 0, 256);
  ShootHandle = osThreadCreate(osThread(Shoot), NULL);

  /* definition and creation of AutoAiming */
  osThreadDef(AutoAiming, autoAiming, osPriorityIdle, 0, 256);
  AutoAimingHandle = osThreadCreate(osThread(AutoAiming), NULL);

  /* definition and creation of Gimbal_RS05 */
  osThreadDef(Gimbal_RS05, gimbalControl_RS05, osPriorityIdle, 0, 256);
  Gimbal_RS05Handle = osThreadCreate(osThread(Gimbal_RS05), NULL);

  /* definition and creation of USBTask */
  osThreadDef(USBTask, USB, osPriorityIdle, 0, 256);
  USBTaskHandle = osThreadCreate(osThread(USBTask), NULL);

  /* definition and creation of myTask06 */
  osThreadDef(myTask06, gc_task, osPriorityIdle, 0, 128);
  myTask06Handle = osThreadCreate(osThread(myTask06), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_gimbalControl */
/**
  * @brief  Function implementing the Gimbal thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_gimbalControl */
__weak void gimbalControl(void const * argument)
{
  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN gimbalControl */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END gimbalControl */
}

/* USER CODE BEGIN Header_shootControl */
/**
* @brief Function implementing the Shoot thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_shootControl */
__weak void shootControl(void const * argument)
{
  /* USER CODE BEGIN shootControl */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END shootControl */
}

/* USER CODE BEGIN Header_autoAiming */
/**
* @brief Function implementing the AutoAiming thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_autoAiming */
__weak void autoAiming(void const * argument)
{
  /* USER CODE BEGIN autoAiming */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END autoAiming */
}

/* USER CODE BEGIN Header_gimbalControl_RS05 */
/**
* @brief Function implementing the Gimbal_RS05 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_gimbalControl_RS05 */
__weak void gimbalControl_RS05(void const * argument)
{
  /* USER CODE BEGIN gimbalControl_RS05 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END gimbalControl_RS05 */
}

/* USER CODE BEGIN Header_USB */
/**
* @brief Function implementing the USBTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_USB */
__weak void USB(void const * argument)
{
  /* USER CODE BEGIN USB */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END USB */
}

/* USER CODE BEGIN Header_gc_task */
/**
* @brief Function implementing the myTask06 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_gc_task */
__weak void gc_task(void const * argument)
{
  /* USER CODE BEGIN gc_task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END gc_task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
