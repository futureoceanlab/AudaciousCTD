#include "flash.h"
#include "UART.h"
FLASH_EraseInitTypeDef EraseInitStruct;
uint32_t PAGEError = 0;
__IO uint32_t  MemoryProgramStatus = 0;

uint16_t Flash_Get_Nth_Page(uint8_t page) {
	return *(__IO uint16_t *)(Flash_Get_Page_Addr(page));
}

uint32_t Flash_Get_Page_Addr(uint8_t page) {
	return FLASH_USER_START_ADDR+FLASH_PAGE_SIZE*page;
}

int8_t Flash_Erase(uint32_t start_addr, uint32_t end_addr, uint32_t n) {

	/* Fill EraseInit structure*/
	EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.PageAddress = start_addr;
	EraseInitStruct.NbPages     = n; //(end_addr - start_addr) / FLASH_PAGE_SIZE;

	if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
	{
		return -1;
	}
	return 1;
}

void Flash_Save_Bulk_Data(struct SensorData *bulk_data, uint8_t len, uint8_t page, uint16_t global_nth_page) {
	// Handle new pages elegantly please!
	HAL_FLASH_Unlock();
	uint32_t start_addr = FLASH_USER_START_ADDR + FLASH_PAGE_SIZE*page;
	// Erase the Page
	Flash_Erase(start_addr, start_addr + FLASH_PAGE_SIZE, 1);
	uint32_t addr = start_addr;
	char m[100];
	sprintf(m, "save nth=%u to addr=%x\r\n", global_nth_page, addr);
	HAL_UART_Transmit(&huart2, (uint8_t*)m, strlen(m), 1000);
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, addr, global_nth_page);
	addr += 2;
	for (uint8_t i = 0; i < len; i++) {
		struct SensorData s = bulk_data[i];
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr,
							  s.Timestamp) == HAL_OK)
		{
			addr = addr + 4;
		}
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr,
							  s.Idx) == HAL_OK)
		{
			addr = addr + 4;
		}
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr,
							  (s.Pressure)) == HAL_OK)
		{
			addr = addr + 4;
		}
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr,
							  (s.Temperature)) == HAL_OK)
		{
			addr = addr + 4;
		}
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr,
							  (s.Conductivity)) == HAL_OK)
		{
			addr = addr + 4;
		}
		//*(uint32_t *)&
//		char mm[100];
//		int n = sprintf(mm, "%u \r\n", i);
//		HAL_UART_Transmit(&huart2, (uint8_t*)mm, strlen(mm), 1000);
//		addr = Flash_Write_Sensor_Data(addr, addr+SENSOR_DATA_SIZE, &s_data);
	}
	HAL_FLASH_Lock();
	// clear the bulk data to zeros
//	char *byte_addr = (char *)bulk_data;
//	for (uint16_t j = 0; j < len*sizeof(struct SensorData); j++) {
//		*byte_addr = 0;
//		byte_addr += 1;
//	}
}

// TODO: Test this function
void Flash_Read_Page_Sensor_Data(uint8_t page, struct SensorData s_data[], uint32_t len) {
	uint32_t addr = Flash_Get_Page_Addr(page);
	uint16_t cur_nth_page = Flash_Get_Nth_Page(addr);
	if (cur_nth_page == 0xFFFF) return;

	addr += 2;
	for (uint32_t i = 0; i < len; i++) {
		Flash_Read_Sensor_Data(addr, &(s_data[i]));
	}
}

uint32_t Flash_Read_Sensor_Data(uint32_t start_addr, struct SensorData *s_data) {
	uint32_t cur_addr = start_addr;
	for (int i = 0; i < 5; i++) {
		switch (i) {
			// Read timestamp and idx
		case 0: s_data->Timestamp = *(__IO uint32_t *)cur_addr;
		case 1:	s_data->Idx =  *(__IO uint32_t *)cur_addr;
		case 2: s_data->Pressure =  *(__IO uint32_t *)cur_addr;
		case 3: s_data->Temperature =  *(__IO uint32_t *)cur_addr;
		case 4: s_data->Conductivity =  *(__IO uint32_t *)cur_addr;
		}
		cur_addr = cur_addr + 4;
	}
	return cur_addr;
}
