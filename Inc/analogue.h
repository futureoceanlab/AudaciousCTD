
#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f3xx_hal.h"

void MX_ADC1_Init(void);
void MX_DAC1_Init(void);
void MX_ADC2_Init(void);
uint32_t ADC_Measure(ADC_HandleTypeDef* hadc);


ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;

DAC_HandleTypeDef hdac1;

#ifdef __cplusplus
}
#endif
