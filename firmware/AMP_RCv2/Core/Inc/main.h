/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f3xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define MCO_Pin GPIO_PIN_0
#define MCO_GPIO_Port GPIOF
#define BTN_DOWN_Pin GPIO_PIN_1
#define BTN_DOWN_GPIO_Port GPIOF
#define BTN_DOWN_EXTI_IRQn EXTI1_IRQn
#define JOYSTICK_LEFT_H_Pin GPIO_PIN_0
#define JOYSTICK_LEFT_H_GPIO_Port GPIOA
#define JOYSTICK_LEFT_V_Pin GPIO_PIN_1
#define JOYSTICK_LEFT_V_GPIO_Port GPIOA
#define VCP_TX_Pin GPIO_PIN_2
#define VCP_TX_GPIO_Port GPIOA
#define BTN_RIGHT_Pin GPIO_PIN_3
#define BTN_RIGHT_GPIO_Port GPIOA
#define BTN_RIGHT_EXTI_IRQn EXTI3_IRQn
#define JOYSTICK_RIGHT_V_Pin GPIO_PIN_4
#define JOYSTICK_RIGHT_V_GPIO_Port GPIOA
#define JOYSTICK_RIGHT_H_Pin GPIO_PIN_5
#define JOYSTICK_RIGHT_H_GPIO_Port GPIOA
#define JOYSTICK_L_SW_Pin GPIO_PIN_0
#define JOYSTICK_L_SW_GPIO_Port GPIOB
#define JOYSTICK_L_SW_EXTI_IRQn EXTI0_IRQn
#define BAT_CHRG_STAT_Pin GPIO_PIN_1
#define BAT_CHRG_STAT_GPIO_Port GPIOB
#define JOYSTICK_R_SW_Pin GPIO_PIN_8
#define JOYSTICK_R_SW_GPIO_Port GPIOA
#define JOYSTICK_R_SW_EXTI_IRQn EXTI9_5_IRQn
#define LORA_RST_Pin GPIO_PIN_9
#define LORA_RST_GPIO_Port GPIOA
#define LORA_NSS_Pin GPIO_PIN_10
#define LORA_NSS_GPIO_Port GPIOA
#define OLED_NSS_Pin GPIO_PIN_11
#define OLED_NSS_GPIO_Port GPIOA
#define LORA_DIO0_Pin GPIO_PIN_12
#define LORA_DIO0_GPIO_Port GPIOA
#define LORA_DIO0_EXTI_IRQn EXTI15_10_IRQn
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define VCP_RX_Pin GPIO_PIN_15
#define VCP_RX_GPIO_Port GPIOA
#define OLED_RST_Pin GPIO_PIN_4
#define OLED_RST_GPIO_Port GPIOB
#define OLED_DC_Pin GPIO_PIN_5
#define OLED_DC_GPIO_Port GPIOB
#define BTN_LEFT_Pin GPIO_PIN_6
#define BTN_LEFT_GPIO_Port GPIOB
#define BTN_LEFT_EXTI_IRQn EXTI9_5_IRQn
#define BTN_UP_Pin GPIO_PIN_7
#define BTN_UP_GPIO_Port GPIOB
#define BTN_UP_EXTI_IRQn EXTI9_5_IRQn

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
