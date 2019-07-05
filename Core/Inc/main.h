/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#include "stm32f0xx_hal.h"

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
#define BL_POWER_Pin GPIO_PIN_0
#define BL_POWER_GPIO_Port GPIOF
#define KEY_IN3_Pin GPIO_PIN_1
#define KEY_IN3_GPIO_Port GPIOF
#define DAC_POWER_Pin GPIO_PIN_0
#define DAC_POWER_GPIO_Port GPIOA
#define KEY_IN1_Pin GPIO_PIN_1
#define KEY_IN1_GPIO_Port GPIOA
#define KEY_IN2_Pin GPIO_PIN_4
#define KEY_IN2_GPIO_Port GPIOA
#define LCD_SCL_Pin GPIO_PIN_5
#define LCD_SCL_GPIO_Port GPIOA
#define LCD_RS_Pin GPIO_PIN_6
#define LCD_RS_GPIO_Port GPIOA
#define LCD_SDA_Pin GPIO_PIN_7
#define LCD_SDA_GPIO_Port GPIOA
#define LCD_RESET_Pin GPIO_PIN_0
#define LCD_RESET_GPIO_Port GPIOB
#define LCD_CS_Pin GPIO_PIN_1
#define LCD_CS_GPIO_Port GPIOB
#define LCD_POWER_Pin GPIO_PIN_8
#define LCD_POWER_GPIO_Port GPIOA
#define DAC_MUTE_Pin GPIO_PIN_12
#define DAC_MUTE_GPIO_Port GPIOA
#define FM_POWER_Pin GPIO_PIN_5
#define FM_POWER_GPIO_Port GPIOB
#define FM_SCL_Pin GPIO_PIN_6
#define FM_SCL_GPIO_Port GPIOB
#define FM_SDA_Pin GPIO_PIN_7
#define FM_SDA_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
//----------LCD接口----------------
#define LCD_POWER_ON   HAL_GPIO_WritePin(LCD_POWER_GPIO_Port, LCD_POWER_Pin, GPIO_PIN_RESET)
#define LCD_POWER_OFF  HAL_GPIO_WritePin(LCD_POWER_GPIO_Port, LCD_POWER_Pin, GPIO_PIN_SET)
#define LCD_SCL_CLR    HAL_GPIO_WritePin(LCD_SCL_GPIO_Port, LCD_SCL_Pin, GPIO_PIN_RESET)
#define LCD_SCL_SET    HAL_GPIO_WritePin(LCD_SCL_GPIO_Port, LCD_SCL_Pin, GPIO_PIN_SET)
#define LCD_SDA_CLR    HAL_GPIO_WritePin(LCD_SDA_GPIO_Port, LCD_SDA_Pin, GPIO_PIN_RESET)
#define LCD_SDA_SET    HAL_GPIO_WritePin(LCD_SDA_GPIO_Port, LCD_SDA_Pin, GPIO_PIN_SET)
#define LCD_RESET_CLR  HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_RESET)
#define LCD_RESET_SET  HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_SET)
#define LCD_RS_CLR     HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_RESET)
#define LCD_RS_SET     HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_SET)
#define LCD_CS_CLR     HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET)
#define LCD_CS_SET     HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET)
//----------其他电源控制----------------
#define BL_POWER_ON    HAL_GPIO_WritePin(BL_POWER_GPIO_Port, BL_POWER_Pin, GPIO_PIN_RESET)
#define BL_POWER_OFF   HAL_GPIO_WritePin(BL_POWER_GPIO_Port, BL_POWER_Pin, GPIO_PIN_SET)
#define FM_POWER_ON    HAL_GPIO_WritePin(FM_POWER_GPIO_Port, FM_POWER_Pin, GPIO_PIN_RESET)
#define FM_POWER_OFF   HAL_GPIO_WritePin(FM_POWER_GPIO_Port, FM_POWER_Pin, GPIO_PIN_SET)
#define BL_POWER_ON    HAL_GPIO_WritePin(BL_POWER_GPIO_Port, BL_POWER_Pin, GPIO_PIN_RESET)
#define BL_POWER_OFF   HAL_GPIO_WritePin(BL_POWER_GPIO_Port, BL_POWER_Pin, GPIO_PIN_SET)
#define DAC_POWER_ON   HAL_GPIO_WritePin(DAC_POWER_GPIO_Port, DAC_POWER_Pin, GPIO_PIN_RESET)
#define DAC_POWER_OFF  HAL_GPIO_WritePin(DAC_POWER_GPIO_Port, DAC_POWER_Pin, GPIO_PIN_SET)
#define DAC_MTUE_ON    HAL_GPIO_WritePin(DAC_MUTE_GPIO_Port, DAC_MUTE_Pin, GPIO_PIN_RESET)
#define DAC_MTUE_OFF   HAL_GPIO_WritePin(DAC_MUTE_GPIO_Port, DAC_MUTE_Pin, GPIO_PIN_SET)
#define TEST_FLASH     HAL_GPIO_TogglePin(TEXT_GPIO_Port, TEXT_Pin)
//--------------按键----------------
#define KEY_BL      HAL_GPIO_ReadPin(KEY_IN1_GPIO_Port,KEY_IN1_Pin)
#define KEY_FM      HAL_GPIO_ReadPin(KEY_IN3_GPIO_Port,KEY_IN3_Pin)
#define KEY_MAIN    HAL_GPIO_ReadPin(KEY_IN2_GPIO_Port,KEY_IN2_Pin)

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
