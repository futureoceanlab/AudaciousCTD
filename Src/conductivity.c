#include "UART.h"
#include "timer.h"
#include "analogue.h"
#include "conductivity.h"
#include "string.h"

/**
 * Call to Measure Conductivity averaged over SAMPLE_SIZE
 * Returns (long) Conductivity Measure * 10000
 */
int32_t Conductivity_Measure_Average(void)
{
	// Reset the variables
	sample_num = 0;
	conductivity = 0;
	voltage_high = 0;
	current_high = 0;
	voltage_low = 0;
	current_low = 0;
	voltage_cal = 0;
	current_cal = 0;

	// Start DAC, PWM interrupts and ADC channels
	HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, DAC_OUT);
	MX_TIM4_Init();
	if (HAL_TIM_Base_Start_IT(&htim4) != HAL_OK) Error_Handler();
	HAL_DAC_Start(&hdac1, DAC_CHANNEL_1);

	// wait for 10 sample number of sample - 1, with count 0 */
	// Sampling High and Low, Sample size needs plus one and double

	while(sample_num < (SAMPLE_SIZE +1) *2 ){
	};

	//Turn DAC and ADCs off
	HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 0);
	HAL_ADC_Stop(&hadc1);
	HAL_ADC_Stop(&hadc2);
//	HAL_TIM_Base_DeInit(&htim4);

	// If out of loop done sampling, calculates the average Conductivity Sample*/
	conductivity_avg = conductivity/(SAMPLE_SIZE);

	return conductivity_avg;
}

/**
 * Call to check if Conductivity Sensor is in range of (10mS to 50mS)
 * Returns 1 if the Conductivity Sensor is measuring high conductivity
 * else returns 0
 */
int Conductivity_check(void){
	if (Conductivity_Measure_Average() > ((uint32_t) 100000)){
		return 1;
	}
	else{
		return 0;
	}
}

/**
 * Call to Measure Conductivity Once
 * Use inside interrupts, conductivity values are added together
 */
void Conductivity_Measure_Single(void)
{
	//Toggle PWM Square wave
	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_12);

	// ADC sample on High side //
	if (!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12)){
		// Short wait for equilibrium
		// HAL_Delay(1);

		// Measure 5 times
		int i;
		uint32_t voltage = 0;
		uint32_t current = 0;
	    for(i = 0; i < 5; i++)
	    {
			voltage = voltage + ADC_Measure(&hadc1);
			current = current + ADC_Measure(&hadc2);
	    }

	    // Average values then scale by 10
	    voltage_high = voltage*10/5;
	    current_high = current*10/5;
	}
	// ADC sample on Low side //
	else if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12)){
		// Short wait for equilibrium
		// HAL_Delay(1);

		// Measure 5 times
		int i = 0;
		uint32_t voltage = 0;
		uint32_t current = 0;
	    for(i = 0; i < 5; i++)
	    {
			voltage = voltage + ADC_Measure(&hadc1);
			current = current + ADC_Measure(&hadc2);
	    }

	    // Average values then scale by 10
		voltage_low = voltage*10/5;
		current_low = current*10/5;

		// Conductivity calculation
		voltage_cal = voltage_high - voltage_low;
		current_cal = current_high - current_low;
		conductivity = conductivity + current_cal*100000/ voltage_cal;
	}
	// Keep track of number of sample
	sample_num ++ ;
}
