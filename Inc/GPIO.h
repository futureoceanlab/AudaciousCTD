#ifdef __cplusplus
extern "C" {
#endif


#include "stm32f3xx_hal.h"

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

void MX_GPIO_Init(void);

#ifdef __cplusplus
}
#endif
