#ifndef HCSR04_H
#define HCSR04_H

#include "pinmap.h"

extern void HCSR04_TrigPinPWMInit(pinStruct_t& trigPin,
																  TIM_TypeDef* TIMx,
																  uint8_t gpioAFSelTIMx,
																  uint8_t pwmChannel);

class HCSR04
{
	private:
		TIM_HandleTypeDef htim;
		uint32_t pulseWidth;
		uint32_t distanceCM;
	
	public:
			HCSR04(pinStruct_t& echoPin,
						 TIM_TypeDef* TIMx,
						 uint8_t gpioAFSelTIMx);
	
			uint32_t GetDistance(void);
			
};

#endif //HCSR04_H
