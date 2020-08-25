#include "temperature.h"
#include "UART.h"
#include "timer.h"
#include "I2C.h"

#define Si7051_ADDR             0x40
#define Si7051_RESET			0xFE
#define Si7051_READ_HOLD		0xE3
#define Si7051_READ_NOHOLD		0xF3


float Temperature_Measure() {
	// Reset the Sensor
	uint8_t outbuffer = Si7051_RESET;
	HAL_I2C_Master_Transmit(&hi2c2, Si7051_ADDR<<1, &outbuffer, 1, 1000);
	// reset completion waiting
	HAL_Delay(10);

	outbuffer = Si7051_READ_NOHOLD;
	HAL_I2C_Master_Transmit(&hi2c2, Si7051_ADDR<<1, &outbuffer, 1, 1000);
	uint8_t buffer[3];
	HAL_StatusTypeDef rx_success = HAL_ERROR;
	uint8_t i = 0;
	while (rx_success != HAL_OK && i < 10) {
		rx_success = HAL_I2C_Master_Receive(&hi2c2, Si7051_ADDR<<1, buffer, 3, 1000);
		HAL_Delay(5);
		i++;
	}
	uint16_t T_code = buffer[0]<<8 | buffer[1];
	uint8_t crc = buffer[2];

	float temperature = ((175.72f * ((float)T_code)) / 65536.0f) - 46.85f;
//	char m[40];
//	sprintf(m, "temperature: %d.%d\r\n", (int)temperature, ((int)(temperature*100))%100);
//	HAL_UART_Transmit(&huart2, (uint8_t*)m, strlen(m), 1000);
	return temperature;

}



// Below is the code for TSIC501, ZAC Wire *****************************************

/*
uint32_t D[32];
uint32_t i = 0;
uint32_t d = 0;

void Temperature_Measure(TIM_HandleTypeDef *htim)
{
  if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
  {
	__IO uint32_t uwDutyCycle = 0;
	__IO uint32_t uwIC2Value = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);

	if (uwIC2Value != 0)
	{
	  uwDutyCycle = 100 - (HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2))*100 / uwIC2Value;
	  if (uwDutyCycle > 95) {
		  // completed 16 bits of data conversion
		  if (d > 0 && i == 16) {
			  temperature_finished = 1;
			  temperature = (((float)d) / 2047) * 70 - 10;
//			  char m[100];
//			  int n = sprintf(m, "Temp: %u.%u, i: %u \r\n", (uint32_t)temp,\
			  ((uint32_t)(temp*100))%100, i);
//			  HAL_UART_Transmit(&huart2, (uint8_t*)m, strlen(m), 1000);
		  }
		  i = 0;
		  d = 0;
	  } else if (( uwDutyCycle > 20) && (uwDutyCycle < 30)) {
		  // bit 0
		  d = (d << 1) ;
		  i++;
	  } else if ((uwDutyCycle > 70) && (uwDutyCycle < 80)) {
		  // bit 1
		  d = (d << 1) | 1;
		  i++;
	  }
	}
  }
}

void Temperature_Enable(void) {
	if (HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_1) != HAL_OK)
	{
	 Starting Error
	Error_Handler();
	}
	if (HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_2) != HAL_OK)
	{
	 Starting Error
	Error_Handler();
	}
	i = 0;
	d = 0;
}

void Temperature_Disable(void) {
	if (HAL_TIM_IC_Stop_IT(&htim3, TIM_CHANNEL_1) != HAL_OK)
	{
	 Starting Error
	Error_Handler();
	}
	if (HAL_TIM_IC_Stop_IT(&htim3, TIM_CHANNEL_2) != HAL_OK)
	{
	 Starting Error
	Error_Handler();
	}
	i = 0;
	d = 0;
}
*/
