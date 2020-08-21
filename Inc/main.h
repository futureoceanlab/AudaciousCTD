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
#include "stm32f3xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
extern uint32_t ts;
extern uint8_t cur_page, unlogged_start_page;

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
uint8_t Check_System(void);
uint8_t Check_Sensors(void);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define Current_ADC_Pin GPIO_PIN_1
#define Current_ADC_GPIO_Port GPIOC
#define Voltage_ADC_Pin GPIO_PIN_2
#define Voltage_ADC_GPIO_Port GPIOC
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define ZAC_Wire_Pin GPIO_PIN_6
#define ZAC_Wire_GPIO_Port GPIOC
#define PWM_Vext_Pin GPIO_PIN_12
#define PWM_Vext_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define SYS_STATUS_Pin GPIO_PIN_3
#define SYS_STATUS_GPIO_Port GPIOB
#define RPI_Ready_Pin GPIO_PIN_4
#define RPI_Ready_GPIO_Port GPIOB
#define RPI_Shutdown_Pin GPIO_PIN_5
#define RPI_Shutdown_GPIO_Port GPIOB
#define RPI_Pwr_Enable_Pin GPIO_PIN_8
#define RPI_Pwr_Enable_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
