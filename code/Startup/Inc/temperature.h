#ifdef __cplusplus
extern "C" {
#endif


#include "stm32f3xx_hal.h"

static uint8_t temperature_finished = 0;
static float temperature = -273.0f;

//void Temperature_Enable(void);
//void Temperature_Disable(void);
//void Temperature_Measure(TIM_HandleTypeDef *htim);
float Temperature_Measure(void);


#ifdef __cplusplus
}
#endif
