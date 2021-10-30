#ifndef MOTION_CONTROL_H
#define MOTION_CONTROL_H

#include "hcsr04.h"
#include "motor.h"

extern void MoveToFreePath(HCSR04& left,
													 HCSR04& mid,
													 HCSR04& right,
													 const uint32_t& minObstacleDistance,
													 Motor& leftMotor,
													 Motor& rightMotor);

#endif //MOTION_CONTROL_H
