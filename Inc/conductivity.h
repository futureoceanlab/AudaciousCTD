#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f3xx_hal.h"

// Conductivity Measurement Calibration Variable
#define DAC_OUT ((uint32_t) 373)
#define Cond_Constant ((uint32_t) 1)

// Sample Size for averaging
#define SAMPLE_SIZE ((int) 10)

// Conductivity Function & Variables
int32_t Conductivity_Measure_Average(void);
int Conductivity_check(void);
void Conductivity_Measure_Single(void);
uint32_t ADC_Measure(ADC_HandleTypeDef* hadc);

// Calculation Variables inside Interrupts
volatile int sample_num;
volatile int32_t conductivity;
volatile uint32_t voltage_high;
volatile uint32_t current_high;
volatile uint32_t voltage_low;
volatile uint32_t current_low;
volatile int32_t voltage_cal;
volatile int32_t current_cal;
int32_t conductivity_avg;

#ifdef __cplusplus
}
#endif
