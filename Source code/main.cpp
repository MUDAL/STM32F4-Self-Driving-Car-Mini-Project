#include "main.h"

int main(void)
{
	HAL_Init();
  __HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_TIM1_CLK_ENABLE();
	__HAL_RCC_TIM2_CLK_ENABLE();
	__HAL_RCC_TIM3_CLK_ENABLE();
	__HAL_RCC_TIM4_CLK_ENABLE();
	
	//Trigger pin initialization for HCSR04 sensors
	pinStruct_t trigPin = {GPIOA,GPIO_PIN_10};
	HCSR04_TrigPinPWMInit(trigPin,TIM1,GPIO_AF1_TIM1,TIM_CHANNEL_3);
	
	pinStruct_t echoPin1 = {GPIOA,GPIO_PIN_8};
	pinStruct_t echoPin2 = {GPIOA,GPIO_PIN_0};
	pinStruct_t echoPin3 = {GPIOA,GPIO_PIN_6};
	
	pinStruct_t leftMotorPin1 = {GPIOC,GPIO_PIN_0};
	pinStruct_t leftMotorPin2 = {GPIOC,GPIO_PIN_1};
	pinStruct_t leftMotorPwmPin = {GPIOB,GPIO_PIN_6};
	
	pinStruct_t rightMotorPin1 = {GPIOC,GPIO_PIN_2};
	pinStruct_t rightMotorPin2 = {GPIOC,GPIO_PIN_3};
	pinStruct_t rightMotorPwmPin = {GPIOB,GPIO_PIN_7};
	
	static HCSR04 middleSensor(echoPin1,TIM1,GPIO_AF1_TIM1);
	static HCSR04 leftSensor(echoPin2,TIM2,GPIO_AF1_TIM2);
	static HCSR04 rightSensor(echoPin3,TIM3,GPIO_AF2_TIM3);
	
	static Motor leftMotor(leftMotorPin1,
												 leftMotorPin2,
												 leftMotorPwmPin,
												 TIM4,
												 GPIO_AF2_TIM4,
												 TIM_CHANNEL_1);
	
	static Motor rightMotor(rightMotorPin1,
												  rightMotorPin2,
												  rightMotorPwmPin,
												  TIM4,
												  GPIO_AF2_TIM4,
													TIM_CHANNEL_2);	

	const uint32_t minObstacleDistance = 40;	
	
	while(1)
	{
		MoveToFreePath(leftSensor,
									 middleSensor,
									 rightSensor,
									 minObstacleDistance,
									 leftMotor,
									 rightMotor);		
	}
}

extern "C" void SysTick_Handler(void)
{
	HAL_IncTick();
}

