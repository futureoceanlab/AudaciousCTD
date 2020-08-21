#include "RPI.h"
#include "flash.h"
#include "GPIO.h"
#include "I2C.h"
#include "main.h"
#include "UART.h"


void RPI_On(void);
void RPI_Off(void);
int8_t RPI_Log_Data(uint8_t page, uint16_t nth_page);
int8_t RPI_Log_Bulk_Data_Ascend(uint8_t start_page, uint8_t end_page);
int8_t RPI_Initiate(uint8_t pages);

uint16_t I2C_array_len = 0;

int8_t RPI_Log_Bulk_Data(uint8_t start_page, uint8_t end_page) {
	int8_t RPI_success;
	if (start_page == end_page) {
		// No pages to write, so just sync ts
		uint8_t n_pages = end_page - start_page;
		// Initiate conversation with RPI
		if ((RPI_success = RPI_Initiate(n_pages)) == -1) {
			return -1;
		}
	} else if (end_page < start_page) {
		if ((RPI_success = RPI_Log_Bulk_Data_Ascend(start_page, FLASH_TOTAL_DATA_PAGES)) == -1) {
			return -1;
		}
		if ((RPI_success = RPI_Log_Bulk_Data_Ascend(0, end_page-1)) == -1) {
			return -1;
		}
	} else {
		if ((RPI_success = RPI_Log_Bulk_Data_Ascend(start_page, end_page-1)) == -1) {
			return -1;
		}
	}
	return 1;
}


int8_t RPI_Log_Bulk_Data_Ascend(uint8_t start_page, uint8_t end_page) {
	if (end_page < start_page) return -1;
	int8_t RPI_success;
	uint32_t temp_addr;
	uint8_t n_pages = end_page - start_page + 1;
	// Initiate conversation with RPI
	if ((RPI_success = RPI_Initiate(n_pages)) == -1) {
		return -1;
	}
	char mm[100];
	uint8_t logged_pages[n_pages];
	uint8_t logged_n = 0;
	// Transfer data to RPI
	uint8_t page = start_page;
	while (page <= end_page) {
		uint16_t cur_nth_page = Flash_Get_Nth_Page(page);
		if (cur_nth_page != 0xFFFF) {
			sprintf(mm, "Copy page: %u\r\n", page);
			HAL_UART_Transmit(&huart2, (uint8_t*)mm, strlen(mm), 1000);

		// the page has data on it, so go ahead and log
			if ((RPI_success = RPI_Log_Data(page, cur_nth_page)) == -1) {
				/* Erase Flash Pages whose data are transferred successfully */
				HAL_FLASH_Unlock();
				for (uint8_t j = 0; j < logged_n; j++) {
					temp_addr = Flash_Get_Page_Addr(logged_pages[j]);
					int8_t success = Flash_Erase(temp_addr, temp_addr+FLASH_PAGE_SIZE, 1);
					sprintf(mm, "Erase %u %u successfully: %d\r\n", logged_pages[j], temp_addr, success);
					HAL_UART_Transmit(&huart2, (uint8_t*)mm, strlen(mm), 1000);
				}
				HAL_FLASH_Lock();
				return -1;
			}
			logged_pages[logged_n] = page;
			logged_n++;
		}
		page++;
	}
	/* Erase Flash Pages whose data are transferred successfully */
	// JUNSU: RPI has not been reliable, so we are not going to delete anything
	// on the flash for now
	/*HAL_FLASH_Unlock();
	for (uint8_t j = 0; j < logged_n; j++) {
		temp_addr = Flash_Get_Page_Addr(logged_pages[j]);
		int8_t success = Flash_Erase(temp_addr, temp_addr+FLASH_PAGE_SIZE, 1);
		sprintf(mm, "Erase %u %u successfully: %d\r\n", logged_pages[j], temp_addr, success);
		HAL_UART_Transmit(&huart2, (uint8_t*)mm, strlen(mm), 1000);
	}
	HAL_FLASH_Lock();*/
	return 1;
}

uint32_t RPI_Get_Time(uint32_t *ts) {
	return *ts+1;
}


void RPI_On(void) {
	HAL_GPIO_WritePin(GPIOB, RPI_Shutdown_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, RPI_Pwr_Enable_Pin, GPIO_PIN_RESET);
	RPI_Status = RPI_PREPARING;
}


void RPI_Off(void) {
	HAL_GPIO_WritePin(GPIOB, RPI_Shutdown_Pin, RESET);
	HAL_Delay(5000);
	HAL_GPIO_WritePin(GPIOB, RPI_Shutdown_Pin, SET);

	HAL_GPIO_WritePin(GPIOB, RPI_Pwr_Enable_Pin, SET);
	char *m = "RPI is off now\r\n";
	HAL_UART_Transmit(&huart2, (uint8_t*)m, strlen(m), 1000);

	RPI_Status = RPI_OFF;
}

int8_t RPI_Initiate(uint8_t pages) {
	char *m;
	// Receive from RPI its ts
	uint32_t rx;
	char mm[100];
	sprintf(mm, "Initialize RPI conv with %u pages\r\n", pages);
	HAL_UART_Transmit(&huart2, (uint8_t*)mm, strlen(mm), 1000);

	HAL_StatusTypeDef s = HAL_I2C_Slave_Receive(&hi2c1, (uint8_t *)&rx, (uint16_t) 4, 10000);
	m = "Tried and...\r\n";
	HAL_UART_Transmit(&huart2, (uint8_t*)m, strlen(m), 1000);
	if (s != HAL_OK) {
		m = "Failed receiving nth_page from RPI after 100seconds\r\n";
		HAL_UART_Transmit(&huart2, (uint8_t*)m, strlen(m), 1000);
		return -1;
	} else {
		char mm[100];
		ts = rx;
		sprintf(mm, "ts = %u\r\n", ts);
		HAL_UART_Transmit(&huart2, (uint8_t*)mm, strlen(mm), 1000);
	}
	uint8_t p = pages;
	// Send # of pages to be written
	// TODO: What happens when RPI nth_page and STM nth_page do not match?
	if (HAL_I2C_Slave_Transmit(&hi2c1, (uint8_t *)&p, 1, 10000) != HAL_OK) {
		m = "FAIL: n pages transfer\r\n";
		HAL_UART_Transmit(&huart2, (uint8_t*)m, strlen(m), 1000);
		return -1;
	} else {
		m = "SUCCESS: pages transfer\r\n";
		HAL_UART_Transmit(&huart2, (uint8_t*)m, strlen(m), 1000);
	}
//
//	return 1;
	// Get OK from RPI
	uint8_t rx2 = 0;
	if (HAL_I2C_Slave_Receive(&hi2c1, (uint8_t *)&rx2, 1, 10000) != HAL_OK) {
		m = "FAIL: writing to file in RPI after 100seconds\r\n";
		HAL_UART_Transmit(&huart2, (uint8_t*)m, strlen(m), 1000);
		return -1;
	} else {
		char mm[100];
		sprintf(mm, "SUCCESS: RPI - received:%u\r\n", rx2);
		HAL_UART_Transmit(&huart2, (uint8_t*)mm, strlen(mm), 1000);
	}
	return rx2 == 1;
}


int8_t RPI_Log_Data(uint8_t page, uint16_t nth_page) {
	// I2C communication between RPI and STM32
	uint32_t start_addr = Flash_Get_Page_Addr(page);
	uint16_t cur_nth_page = Flash_Get_Nth_Page(start_addr);
	if (cur_nth_page == 0xFFFF) return 1;
	struct SensorData s_data_I2C[MAX_SENSOR_DATA_ARR_LEN];
	Flash_Read_Page_Sensor_Data(page, &s_data_I2C, MAX_SENSOR_DATA_ARR_LEN);
	char mm[100];
	for (int i = 0; i < MAX_SENSOR_DATA_ARR_LEN; i++) {
		struct SensorData s = s_data_array[i];
		sprintf(mm, "%X%X%X%X%X ", s.Idx, s.Timestamp,
				*(unsigned int*)&s.Conductivity, *(unsigned int*)&s.Temperature,
				*(unsigned int*)&s.Pressure);
		HAL_UART_Transmit(&huart2, (uint8_t*)mm, strlen(mm), 1000);
	}
	char *m;
	uint16_t n_bytes = MAX_SENSOR_DATA_BYTES;
//	// Send the page data
//	if (HAL_I2C_Slave_Transmit(&hi2c1, (uint8_t *)&n_bytes, 2, 100000) != HAL_OK) {
//		m = "FAIL: data transfer\r\n";
//		HAL_UART_Transmit(&huart2, (uint8_t*)m, strlen(m), 1000);
//		return -1;
//	} else {
//		m = "SUCCESS: data transfer\r\n";
//		HAL_UART_Transmit(&huart2, (uint8_t*)m, strlen(m), 1000);
//	}
	// Send the page data
	if (HAL_I2C_Slave_Transmit(&hi2c1, (uint8_t *)s_data_I2C, MAX_SENSOR_DATA_BYTES, 100000) != HAL_OK) {
		m = "FAIL: data transfer\r\n";
		HAL_UART_Transmit(&huart2, (uint8_t*)m, strlen(m), 1000);
		return -1;
	} else {
		m = "SUCCESS: data transfer\r\n";
		HAL_UART_Transmit(&huart2, (uint8_t*)m, strlen(m), 1000);
	}
//	uint8_t ok = 0;
//	// Receive OK from RPI
//	if (HAL_I2C_Slave_Receive(&hi2c1, (uint8_t *)&ok, 1, 100000) != HAL_OK) {
//		m = "FAIL: writing to file in RPI after 100seconds\r\n";
//		HAL_UART_Transmit(&huart2, (uint8_t*)m, strlen(m), 1000);
//		return -1;
//	} else {
//		m = "SUCCESS: RPI - write complete\r\n";
//		HAL_UART_Transmit(&huart2, (uint8_t*)m, strlen(m), 1000);
//	}
//	return ok == 1;
	return 1;
}


void RPI_Wifi() {
	// Initiate conversation with RPI with 0 read --> check wifi
	RPI_Initiate(0);
}


void RPI_Ready_Handler(void) {
	// We are ready to log to RPI, set the flag!
	RPI_Status = RPI_ON;
	// Start working on a new page
	int8_t log_success = RPI_Log_Bulk_Data(unlogged_start_page, cur_page);
	char m[100];
	if (log_success == -1) {
	// RPI logging failed
		sprintf(m, "RPI Failed, start from %u%u now\r\n", cur_page, unlogged_start_page);
		HAL_UART_Transmit(&huart2, (uint8_t*)m, strlen(m), 1000);
		if ((cur_page + 1) % FLASH_TOTAL_DATA_PAGES == unlogged_start_page) {
		// RPI is full, flag RPI problem and try to save to RPI
			while (log_success == -1) {
				log_success = RPI_Log_Bulk_Data(unlogged_start_page, cur_page);
			}
			cur_page = 0;
			unlogged_start_page = 0;
		}
		// If there is some space left in flash, just go on to flash and try later
	}
	unlogged_start_page = cur_page;
//	} else {
//	// RPI logging successful, start on new page
//		unlogged_start_page = cur_page;
//		sprintf(m, "RPI succcessful, start from %u%u now\r\n", cur_page, unlogged_start_page);
//		HAL_UART_Transmit(&huart2, (uint8_t*)m, strlen(m), 1000);
//	}
	sprintf(m, "Bye RPI now!\r\n");
	HAL_UART_Transmit(&huart2, (uint8_t*)m, strlen(m), 1000);
	RPI_Off();
}
