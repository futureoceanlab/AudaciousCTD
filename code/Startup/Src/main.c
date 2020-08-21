/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"

#include "analogue.h"
#include "UART.h"
#include "timer.h"
#include "I2C.h"
#include "conductivity.h"
#include "temperature.h"
#include "pressure.h"
#include "flash.h"
#include "RPI.h"
#include "GPIO.h"

/* USER CODE END Includes */


/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MAX_FLASH_PAGE_SIZE FLASH_PAGE_SIZE


/* Private function prototypes -----------------------------------------------*/
void Peripheral_Init(void);
void SystemClock_Config(void);
void MX_GPIO_Init(void);
void Measure_Data(struct SensorData *s_data);
uint8_t Find_Start_End_Page(uint8_t *start_page, uint8_t *end_page);

uint8_t Check_System(void);
uint8_t Check_Data(void);

uint32_t global_idx, cur_addr, ts = 0;
uint8_t cur_page, unlogged_start_page;
uint16_t nth_page;
uint16_t s_data_array_len = 0;


/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
	Peripheral_Init();
	HAL_GPIO_WritePin(GPIOB, RPI_Pwr_Enable_Pin, GPIO_PIN_SET);
//	RPI_Off();
	char *m ="Check Sys\r\n";
	HAL_UART_Transmit(&huart2, (uint8_t*)m, strlen(m), 1000);

	/* System Check Start-------------------------------------------------------- */
	int8_t sys_success = Check_System();

	char mm[100];
	sprintf(mm, "success = %u\r\n", sys_success);
	HAL_UART_Transmit(&huart2, (uint8_t*)m, strlen(m), 1000);

	if (sys_success) {
		HAL_GPIO_WritePin(SYS_STATUS_GPIO_Port, SYS_STATUS_Pin, SET);
		m ="Sys Check Sucecssful\r\n";
		HAL_UART_Transmit(&huart2, (uint8_t*)mm, strlen(mm), 1000);
	} else {
	// Failed
		while (1) {
			HAL_GPIO_TogglePin(SYS_STATUS_GPIO_Port, SYS_STATUS_Pin);
			HAL_Delay(500);
		}
	}
//
//	/* Check if we are in the water or in the air ------------------------------ */
////	uint8_t in_water = Conductivity_check();
////	if (!in_water) {
////		RPI_Wifi(&ts);
////	}
////	while (!Conductivity_cConductivity_Measure_Singleheck()) {}
//	RPI_Off();

	/* Measurement Start-------------------------------------------------------- */
	if (HAL_TIM_Base_Start_IT(&htim2) != HAL_OK) Error_Handler();
	while (1)
	{
	}
}

void Peripheral_Init(void) {
	/* MCU Configuration----------------------------------------------------------*/
	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();
	/* Configure the system clock */
	SystemClock_Config();
	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_ADC1_Init();
	MX_ADC2_Init();
	MX_DAC1_Init();
	MX_I2C1_Init();
	MX_I2C2_Init();
	MX_TIM2_Init();
	MX_TIM3_Init();
	MX_TIM4_Init();
	MX_USART2_UART_Init();
//	HAL_InitTick(12);
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
// Conductivity Measurement Interrupt
	if(TIM4 == htim->Instance) {
		// Sampling High and Low, Sample size needs plus one and double
		// If done sampling, stop tim4 interrupts and DAC
		if (sample_num < (SAMPLE_SIZE +1) *2){
				Conductivity_Measure_Single();
		}
		else{
			HAL_TIM_Base_Stop(&htim4);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, 0);
		}
	} else if (TIM2 == htim->Instance) {
//		HAL_GPIO_TogglePin(SYS_STATUS_GPIO_Port, SYS_STATUS_Pin);
//		if (cur_page < 2) {
		ts++;
		global_idx++;
		char *m = "+";
		HAL_UART_Transmit(&huart2, (uint8_t*)m, strlen(m), 1000);

		struct SensorData s_data;
		Measure_Data(&s_data);
		s_data_array[s_data_array_len] = s_data;
		s_data_array_len++;

		if (s_data_array_len == MAX_SENSOR_DATA_ARR_LEN) {
			int8_t log_successful = -1;
			Flash_Save_Bulk_Data(&s_data_array, s_data_array_len, cur_page, nth_page);
//			if ((RPI_Status == RPI_OFF)
//					&& ((((int8_t)(cur_page - unlogged_start_page)) % 5) == 4)) {
//				// Try to save to RPI every multiple of 5 pages, so turn it on
//				RPI_On();
//			}
//			if (RPI_Status == RPI_ON) {
//				m = "Store at RPI\r\n";
//				HAL_UART_Transmit(&huart2, (uint8_t*)m, strlen(m), 1000);
//				if ((log_successful = RPI_Log_Bulk_Data(unlogged_start_page, cur_page, &ts)) != -1) {
//					unlogged_start_page = cur_page;
//					RPI_Off();
//				}
//			}
			cur_page = (cur_page + 1) % FLASH_TOTAL_DATA_PAGES;
//			if (Flash_Get_Nth_Page(cur_page) != 0xFFFF) {
//				// Reached the first page that needs to be logged
//				// Flash is full, so keep trying to save to RPI
//				// Pause any measurement until then
//				if (RPI_Status == RPI_OFF) RPI_On();
//				if (HAL_TIM_Base_Stop_IT(&htim2) != HAL_OK) Error_Handler();
//				if (HAL_TIM_Base_Stop_IT(&htim4) != HAL_OK) Error_Handler();
//				// Wait until RPI is fully on!
//				while (RPI_Status != RPI_ON) {}
//				// Keep trying to save the file until the end...!
//				while (log_successful == -1) {
//					log_successful = RPI_Log_Bulk_Data(unlogged_start_page, cur_page, &ts);
//				}
//
//				// Restart measuring!
//				if (HAL_TIM_Base_Start_IT(&htim2) != HAL_OK) Error_Handler();
//				if (HAL_TIM_Base_Start_IT(&htim4) != HAL_OK) Error_Handler();
//			}
			nth_page ++;
			s_data_array_len = 0;
			char mm[100];
			sprintf(mm, "len=%u, moved to new page %u\r\n", MAX_SENSOR_DATA_ARR_LEN, cur_page);
			HAL_UART_Transmit(&huart2, (uint8_t*)mm, strlen(mm), 1000);
		}
		/* Upload Done */
//		}
		/* Request to enter SLEEP mode */
//		HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
	} else if(TIM3 == htim->Instance) {
//		Temperature_Measure(htim);
	}
}


void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL2;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_ADC12
                              |RCC_PERIPHCLK_TIM34;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.Adc12ClockSelection = RCC_ADC12PLLCLK_DIV1;
  PeriphClkInit.Tim34ClockSelection = RCC_TIM34CLK_HCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}


uint8_t Check_Data(void) {
	uint8_t start_page = 0, end_page = 0;
	uint8_t new_flash = Find_Start_End_Page(&start_page, &end_page);
	nth_page = Flash_Get_Nth_Page(end_page);
	if (nth_page == 0xFFFF) nth_page = 0;
	char m[100];
	sprintf(m, "unlogged_start=%u, unlogged_end=%u, nth_page = %X\r\n",
			start_page, end_page, nth_page);
	HAL_UART_Transmit(&huart2, (uint8_t*)m, strlen(m), 1000);
	/* RPI Check and flash configuration */
//	RPI_On();
	if (new_flash) {
	// The sensor is brand new
		HAL_UART_Transmit(&huart2, (uint8_t*)m, strlen(m), 1000);

		cur_page = 0;
		unlogged_start_page = 0;
		nth_page = 1;
	} else {
	// Unlogged pages exist, save all the unsaved files to RPI
//		while ((cur_page + 1) %FLASH_TOTAL_DATA_PAGES == unlogged_start_page) {};
		unlogged_start_page = start_page;
		cur_page = (end_page + 1) % FLASH_TOTAL_DATA_PAGES;
		nth_page += 1;
//		char m[100];
//		sprintf(m, "cur_page=%u end_page=%u, nth_page=%u\r\n", cur_page, end_page, nth_page);
//		HAL_UART_Transmit(&huart2, (uint8_t*)m, strlen(m), 1000);
	}
//	while (RPI_Status != RPI_OFF) {};
	char *mm = "RPI is off now for real!!!!\r\n";
	HAL_UART_Transmit(&huart2, (uint8_t*)mm, strlen(m), 1000);
//	cur_page = 0;
//	unlogged_start_page = 0;
//	nth_page = 1;
	return 1;
}

uint8_t Find_Start_End_Page(uint8_t *start_page, uint8_t *end_page) {
	char m[100];
	nth_page = Flash_Get_Nth_Page(0);
	uint8_t page_x=0, page_y=0;
	sprintf(m, "INIT: nth_page = %u\r\n", nth_page);
	HAL_UART_Transmit(&huart2, (uint8_t*)m, strlen(m), 1000);

	if (nth_page != 0xFFFF) {
	// The first page has unlogged data. This could mean (1) 0-x pages are unlogged
	// or (2) 0-x and y-end pages are logged where y is the start and x is the end
	// (3) or 0-end pages are full...!
		char *mm = "Page is not new, search for a new page...\r\n";
		HAL_UART_Transmit(&huart2, (uint8_t*)mm, strlen(mm), 1000);

		while ((nth_page != 0xFFFF) && (page_x < FLASH_TOTAL_DATA_PAGES)) {
			page_x++;
			nth_page = Flash_Get_Nth_Page(page_x);
		}
		page_y = page_x;
		sprintf(m, "page_x = %u\r\n", page_x);
		HAL_UART_Transmit(&huart2, (uint8_t*)m, strlen(m), 1000);

		if (nth_page != 0xFFFF) {
			// case (3) 0-end unlogged pages
			// TODO: The order may actually not from 0 to end
			*start_page = 0;
			*end_page = FLASH_TOTAL_DATA_PAGES;
			mm = "case3: pages are all full and everything was unlogged\r\n";
			HAL_UART_Transmit(&huart2, (uint8_t*)mm, strlen(mm), 1000);
			return 0;
		} else {
			*end_page = page_x-1;
		}
		sprintf(m, "max = %u\r\n", FLASH_TOTAL_DATA_PAGES);
		HAL_UART_Transmit(&huart2, (uint8_t*)m, strlen(m), 1000);

		while ((nth_page == 0xFFFF) && (page_y < FLASH_TOTAL_DATA_PAGES)) {
			page_y++;
			nth_page = Flash_Get_Nth_Page(page_y);
		}
		sprintf(m, "page_y = %u, nth_page=%u\r\n", page_y, nth_page);
		HAL_UART_Transmit(&huart2, (uint8_t*)m, strlen(m), 1000);

		if (nth_page == 0xFFFF) {
			// case (1) 0-x unlogged pages
			*start_page = 0;
		} else {
			// case(2) 0-x and y-end unlogged pages
			*start_page = page_y;
		}
	} else {
	// The first page is untouched. This could mean (1) there is no data at all or
	// (2) x-y pages are unlogged
		while ((nth_page == 0xFFFF) && (page_x < FLASH_TOTAL_DATA_PAGES)) {
			page_x++;
			nth_page = Flash_Get_Nth_Page(end_page);
		}
		if (nth_page == 0xFFFF) {
		// case (1) no data
			*start_page = 0;
			*end_page = 0;
			return 1;
		} else {
			*start_page = page_x;
		}
		page_y = page_x;
		while ((nth_page != 0xFFFF) && (page_y < FLASH_TOTAL_DATA_PAGES)) {
			page_y++;
			nth_page = Flash_Get_Nth_Page(end_page);
		}
		*end_page = nth_page != 0xFFFF ? FLASH_TOTAL_DATA_PAGES: page_y - 1;
	}
	return 0;
}

uint8_t Check_System(void) {
	uint8_t sys_success = 0;
	/* Check the sensor validity-------------------------------------------------- */
	uint8_t sensor_success = 1;// Check_Sensors();
	uint8_t rpi_success = Check_Data();
	char *m = "Hello\r\n";
	HAL_UART_Transmit(&huart2, (uint8_t*)m, strlen(m), 1000);
	sys_success = (sensor_success << 1) +  rpi_success;
	HAL_UART_Transmit(&huart2, (uint8_t*)m, strlen(m), 1000);
	return sys_success;
}

void Measure_Data(struct SensorData *s_data) {
//	/* Measure conductivity */
	uint32_t c_data = Conductivity_Measure_Average();

//	/* Measure temperature */
//	Temperature_Enable();
//	while (!temperature_finished) {
//	// Wait until the temperature measurement is complete
//	}
	float t_data = Temperature_Measure(); //temperature;
	uint32_t t_int = (uint32_t)(t_data * 1000);
////	temperature_finished = 0;
////	Temperature_Disable();
//
//	/* Measure pressure */
	int32_t temp_T;
	uint32_t p_data;
//	float p_data = Pressure_Measure();
	Pressure_Measure(&p_data, &temp_T);
	uint32_t p_int = (uint32_t)(p_data*100);
	if (t_data > 80.f || t_data < 1.f) {
		t_int = (uint32_t) temp_T;
	}
	char m[100];
	int n = sprintf(m, "TS: %u, idx:%u C:%u\tT:%u.%u\tP:%u.%u\r\n",\
				ts, global_idx, c_data, (uint32_t)t_int/1000,\
			  ((uint32_t)(t_int))%1000, (uint32_t)p_int/1000, \
			  ((uint32_t)(p_int))%1000);
	HAL_UART_Transmit(&huart2, (uint8_t*)m, strlen(m), 1000);

//
//	/* Save the data to the flash */
//	// RTC ts, idx, start_addr, end_addr managing needed
//	// FLASH_USER_END_ADDR needs to be specified
	s_data->Idx = global_idx;
	s_data->Timestamp = ts;
	s_data->Conductivity = c_data;
	s_data->Temperature = t_int;
	s_data->Pressure =  p_int;
}


/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(char *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
