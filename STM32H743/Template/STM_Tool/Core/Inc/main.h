/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

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
#define K1_Pin GPIO_PIN_2
#define K1_GPIO_Port GPIOC
#define K2_Pin GPIO_PIN_3
#define K2_GPIO_Port GPIOC
#define Motor_PWM1_Pin GPIO_PIN_0
#define Motor_PWM1_GPIO_Port GPIOA
#define Motor_PWM2_Pin GPIO_PIN_1
#define Motor_PWM2_GPIO_Port GPIOA
#define HongWai_Pin GPIO_PIN_4
#define HongWai_GPIO_Port GPIOA
#define Encoder2_1_Pin GPIO_PIN_6
#define Encoder2_1_GPIO_Port GPIOA
#define Encoder2_2_Pin GPIO_PIN_7
#define Encoder2_2_GPIO_Port GPIOA
#define BIN2_Pin GPIO_PIN_4
#define BIN2_GPIO_Port GPIOC
#define AIN2_Pin GPIO_PIN_5
#define AIN2_GPIO_Port GPIOC
#define BIN1_Pin GPIO_PIN_0
#define BIN1_GPIO_Port GPIOB
#define AIN1_Pin GPIO_PIN_1
#define AIN1_GPIO_Port GPIOB
#define RED_Pin GPIO_PIN_2
#define RED_GPIO_Port GPIOB
#define Buzzer_Pin GPIO_PIN_7
#define Buzzer_GPIO_Port GPIOE
#define YELLOW_Pin GPIO_PIN_8
#define YELLOW_GPIO_Port GPIOE
#define Encoder1_1_Pin GPIO_PIN_9
#define Encoder1_1_GPIO_Port GPIOE
#define GREEN_Pin GPIO_PIN_10
#define GREEN_GPIO_Port GPIOE
#define Encoder1_2_Pin GPIO_PIN_11
#define Encoder1_2_GPIO_Port GPIOE
#define Screen_SCL_Pin GPIO_PIN_10
#define Screen_SCL_GPIO_Port GPIOB
#define Screen_SDA_Pin GPIO_PIN_11
#define Screen_SDA_GPIO_Port GPIOB
#define SMotor_Turn1_Pin GPIO_PIN_12
#define SMotor_Turn1_GPIO_Port GPIOB
#define SMotor_Turn2_Pin GPIO_PIN_13
#define SMotor_Turn2_GPIO_Port GPIOB
#define SMotor_PWM1_Pin GPIO_PIN_14
#define SMotor_PWM1_GPIO_Port GPIOB
#define SMotor_PWM2_Pin GPIO_PIN_15
#define SMotor_PWM2_GPIO_Port GPIOB
#define Servo_PWM1_Pin GPIO_PIN_12
#define Servo_PWM1_GPIO_Port GPIOD
#define Servo_PWM2_Pin GPIO_PIN_13
#define Servo_PWM2_GPIO_Port GPIOD
#define Servo_PWM3_Pin GPIO_PIN_14
#define Servo_PWM3_GPIO_Port GPIOD
#define Servo_PWM4_Pin GPIO_PIN_15
#define Servo_PWM4_GPIO_Port GPIOD
#define Visual_TX_Pin GPIO_PIN_6
#define Visual_TX_GPIO_Port GPIOC
#define Visual_RX_Pin GPIO_PIN_7
#define Visual_RX_GPIO_Port GPIOC
#define IMU_TX_Pin GPIO_PIN_5
#define IMU_TX_GPIO_Port GPIOD
#define IMU_RX_Pin GPIO_PIN_6
#define IMU_RX_GPIO_Port GPIOD
#define Track_Key_Pin GPIO_PIN_5
#define Track_Key_GPIO_Port GPIOB
#define Track_SCL_Pin GPIO_PIN_6
#define Track_SCL_GPIO_Port GPIOB
#define Track_SDA_Pin GPIO_PIN_7
#define Track_SDA_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
