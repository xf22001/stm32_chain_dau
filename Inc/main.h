/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
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
#include "stm32f4xx_hal.h"

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
#define in1_Pin GPIO_PIN_0
#define in1_GPIO_Port GPIOA
#define in2_Pin GPIO_PIN_1
#define in2_GPIO_Port GPIOA
#define in3_Pin GPIO_PIN_2
#define in3_GPIO_Port GPIOA
#define in4_Pin GPIO_PIN_3
#define in4_GPIO_Port GPIOA
#define set1_Pin GPIO_PIN_7
#define set1_GPIO_Port GPIOE
#define set2_Pin GPIO_PIN_8
#define set2_GPIO_Port GPIOE
#define set3_Pin GPIO_PIN_9
#define set3_GPIO_Port GPIOE
#define set4_Pin GPIO_PIN_10
#define set4_GPIO_Port GPIOE
#define rey1_Pin GPIO_PIN_11
#define rey1_GPIO_Port GPIOE
#define rey2_Pin GPIO_PIN_12
#define rey2_GPIO_Port GPIOE
#define rey3_Pin GPIO_PIN_13
#define rey3_GPIO_Port GPIOE
#define rey4_Pin GPIO_PIN_14
#define rey4_GPIO_Port GPIOE
#define rey5_Pin GPIO_PIN_15
#define rey5_GPIO_Port GPIOE
#define kg9_Pin GPIO_PIN_10
#define kg9_GPIO_Port GPIOB
#define kg10_Pin GPIO_PIN_11
#define kg10_GPIO_Port GPIOB
#define fb1_Pin GPIO_PIN_8
#define fb1_GPIO_Port GPIOD
#define fb2_Pin GPIO_PIN_9
#define fb2_GPIO_Port GPIOD
#define fb3_Pin GPIO_PIN_10
#define fb3_GPIO_Port GPIOD
#define fb4_Pin GPIO_PIN_11
#define fb4_GPIO_Port GPIOD
#define kg1_Pin GPIO_PIN_12
#define kg1_GPIO_Port GPIOD
#define kg2_Pin GPIO_PIN_13
#define kg2_GPIO_Port GPIOD
#define kg3_Pin GPIO_PIN_14
#define kg3_GPIO_Port GPIOD
#define kg4_Pin GPIO_PIN_15
#define kg4_GPIO_Port GPIOD
#define kg5_Pin GPIO_PIN_6
#define kg5_GPIO_Port GPIOC
#define kg6_Pin GPIO_PIN_7
#define kg6_GPIO_Port GPIOC
#define kg7_Pin GPIO_PIN_8
#define kg7_GPIO_Port GPIOC
#define kg8_Pin GPIO_PIN_9
#define kg8_GPIO_Port GPIOC
#define ledmcu_Pin GPIO_PIN_15
#define ledmcu_GPIO_Port GPIOA
#define fb9_Pin GPIO_PIN_10
#define fb9_GPIO_Port GPIOC
#define fb10_Pin GPIO_PIN_11
#define fb10_GPIO_Port GPIOC
#define fb5_Pin GPIO_PIN_0
#define fb5_GPIO_Port GPIOD
#define fb6_Pin GPIO_PIN_1
#define fb6_GPIO_Port GPIOD
#define fb7_Pin GPIO_PIN_2
#define fb7_GPIO_Port GPIOD
#define fb8_Pin GPIO_PIN_3
#define fb8_GPIO_Port GPIOD
#define con485_Pin GPIO_PIN_4
#define con485_GPIO_Port GPIOD
#define led485_Pin GPIO_PIN_7
#define led485_GPIO_Port GPIOD
#define ledcan2_Pin GPIO_PIN_4
#define ledcan2_GPIO_Port GPIOB
#define ledcan1_Pin GPIO_PIN_7
#define ledcan1_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
