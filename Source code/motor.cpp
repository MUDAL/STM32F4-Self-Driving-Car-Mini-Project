#include "motor.h"

Motor::Motor(pinStruct_t& motorPin1,
						 pinStruct_t& motorPin2,
						 pinStruct_t& pwmPin,
						 TIM_TypeDef* TIMx,
						 uint8_t gpioAFSelTIMx,
						 uint8_t pwmChannel)
{
	pin1 = motorPin1;
	pin2 = motorPin2;
	pwmCh = pwmChannel;
	
	//GPIO config (PWM pin)
	GPIO_InitTypeDef GPIO_PWMInitStruct = {0};	
	GPIO_PWMInitStruct.Pin = pwmPin.selectedPin;
  GPIO_PWMInitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_PWMInitStruct.Pull = GPIO_NOPULL;
  GPIO_PWMInitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_PWMInitStruct.Alternate = gpioAFSelTIMx;
  HAL_GPIO_Init(pwmPin.port,&GPIO_PWMInitStruct);	
	
	//GPIO config (motor pin1)
	GPIO_InitTypeDef GPIO_Pin1InitStruct = {0};
	GPIO_Pin1InitStruct.Pin = pin1.selectedPin;
	GPIO_Pin1InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_Pin1InitStruct.Pull = GPIO_NOPULL;
  GPIO_Pin1InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(pin1.port,&GPIO_Pin1InitStruct);
	
	//GPIO config (motor pin2)
	GPIO_InitTypeDef GPIO_Pin2InitStruct = {0};
	GPIO_Pin2InitStruct.Pin = pin2.selectedPin;
	GPIO_Pin2InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_Pin2InitStruct.Pull = GPIO_NOPULL;
  GPIO_Pin2InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(pin2.port,&GPIO_Pin2InitStruct);	
	
	//TIM PWM config
  TIM_OC_InitTypeDef sConfigOC = {0};
	htim.Instance = TIMx;
  htim.Init.Prescaler = 10 - 1; 
  htim.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim.Init.Period = 32000 - 1; //20ms period
  htim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim.Init.RepetitionCounter = 0;
  htim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  HAL_TIM_PWM_Init(&htim);
	
	//PWM channel config
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 24000; //default motor speed
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  HAL_TIM_PWM_ConfigChannel(&htim,&sConfigOC,pwmChannel);
	
	//Start PWM signal
	HAL_TIM_PWM_Start(&htim,pwmChannel);	
}

void Motor::MoveForward(uint32_t speed)
{
	__HAL_TIM_SET_COMPARE(&htim,pwmCh,speed);
	HAL_GPIO_WritePin(pin1.port,pin1.selectedPin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(pin2.port,pin2.selectedPin,GPIO_PIN_RESET);
}

void Motor::Reverse(uint32_t speed)
{
	__HAL_TIM_SET_COMPARE(&htim,pwmCh,speed);
	HAL_GPIO_WritePin(pin1.port,pin1.selectedPin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(pin2.port,pin2.selectedPin,GPIO_PIN_SET);
}

void Motor::Stop(void)
{
	HAL_GPIO_WritePin(pin1.port,pin1.selectedPin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(pin2.port,pin2.selectedPin,GPIO_PIN_RESET);	
}

