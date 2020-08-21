#include "GPIO.h"

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();


  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(PWM_Vext_GPIO_Port, PWM_Vext_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, SYS_STATUS_Pin|RPI_Shutdown_Pin|RPI_Pwr_Enable_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : PWM_Vext_Pin */
  GPIO_InitStruct.Pin = PWM_Vext_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(PWM_Vext_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : SYS_STATUS_Pin RPI_Shutdown_Pin RPI_Pwr_Enable_Pin */
  GPIO_InitStruct.Pin = SYS_STATUS_Pin|RPI_Shutdown_Pin|RPI_Pwr_Enable_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : RPI_Ready_Pin */
  GPIO_InitStruct.Pin = RPI_Ready_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(RPI_Ready_GPIO_Port, &GPIO_InitStruct);

  /* Enable and set EXTI line 4 Interrupt to the lowest priority */
//  HAL_NVIC_SetPriority(EXTI4_IRQn, 14, 0);
//  HAL_NVIC_EnableIRQ(EXTI4_IRQn);
}
