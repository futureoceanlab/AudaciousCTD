#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f3xx_hal.h"

#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA


UART_HandleTypeDef huart2;


void MX_USART2_UART_Init(void);


#ifdef __cplusplus
}
#endif
