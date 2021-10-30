#include "hcsr04.h"

void HCSR04_TrigPinPWMInit(pinStruct_t& trigPin,
													 TIM_TypeDef* TIMx,
													 uint8_t gpioAFSelTIMx,
													 uint8_t pwmChannel)
{
	//GPIO config
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = trigPin.selectedPin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = gpioAFSelTIMx;
  HAL_GPIO_Init(trigPin.port,&GPIO_InitStruct);
	
	//TIM PWM config
	TIM_HandleTypeDef htim;
  TIM_OC_InitTypeDef sConfigOC = {0};
	htim.Instance = TIMx;
  htim.Init.Prescaler = 10 - 1; 
  htim.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim.Init.Period = 32000 - 1; //20ms period
  htim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim.Init.RepetitionCounter = 0;
  htim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  HAL_TIM_PWM_Init(&htim);
	
	//PWM channel (trig pin) config
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 24; //15us trigger pulse 
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  HAL_TIM_PWM_ConfigChannel(&htim,&sConfigOC,pwmChannel);
	
	//Start PWM signal
	HAL_TIM_PWM_Start(&htim,pwmChannel);
}

//Member functions
HCSR04::HCSR04(pinStruct_t& echoPin,
							 TIM_TypeDef* TIMx,
							 uint8_t gpioAFSelTIMx)
{
	pulseWidth = 0;
	distanceCM = 0;
	//GPIO config
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = echoPin.selectedPin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = gpioAFSelTIMx;
  HAL_GPIO_Init(echoPin.port,&GPIO_InitStruct);
	
	//TIM Input Capture config
  htim.Instance = TIMx;
  htim.Init.Prescaler = 200 - 1; 
  htim.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim.Init.Period = 16000 - 1; 
  htim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	HAL_TIM_IC_Init(&htim);
	
	//Input capture channel config
	//Slave controller and trigger input selection
	TIM_IC_InitTypeDef sConfigIC = {0};
	TIM_SlaveConfigTypeDef sSlaveConfig = {0};
	
	sSlaveConfig.SlaveMode = TIM_SLAVEMODE_RESET; //configure slave mode controller in reset mode
	sSlaveConfig.InputTrigger = TIM_TS_TI1FP1; //Selecting filtered Timer input 1 as trigger input
	sSlaveConfig.TriggerPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sSlaveConfig.TriggerPrescaler = TIM_ICPSC_DIV1;
	HAL_TIM_SlaveConfigSynchro(&htim,&sSlaveConfig);
	//IC1
	sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0x0F; 
	HAL_TIM_IC_ConfigChannel(&htim,&sConfigIC,TIM_CHANNEL_1);
	//IC2
	sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
  sConfigIC.ICSelection = TIM_ICSELECTION_INDIRECTTI;
	HAL_TIM_IC_ConfigChannel(&htim,&sConfigIC,TIM_CHANNEL_2);
	
	HAL_TIM_IC_Start(&htim,TIM_CHANNEL_1);
	HAL_TIM_IC_Start(&htim,TIM_CHANNEL_2);
}

uint32_t HCSR04::GetDistance(void)
{
	const uint32_t sysClockFreq = 16000000;
	if((htim.Instance->SR & TIM_SR_CC2IF) == TIM_SR_CC2IF)
	{
		pulseWidth = htim.Instance->CCR2;
		distanceCM = (float)pulseWidth * htim.Init.Prescaler * 1000000 / (58 * sysClockFreq);
	}
	return distanceCM;
}

