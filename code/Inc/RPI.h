#ifdef __cplusplus
extern "C" {
#endif


#include "stm32f3xx_hal.h"


/**
  * @brief  RPIL Status structures definition
  */
typedef enum
{
  RPI_OFF      	= 0x00U,
  RPI_PREPARING	= 0x01U,
  RPI_ON     	= 0x02U
} RPI_StatusTypeDef;

int8_t RPI_Log_Bulk_Data(uint8_t start_page, uint8_t end_page);
void RPI_Wifi(void);
void RPI_Ready_Handler(void);
void RPI_On(void);
void RPI_Off(void);

RPI_StatusTypeDef RPI_Status;

#ifdef __cplusplus
}
#endif
