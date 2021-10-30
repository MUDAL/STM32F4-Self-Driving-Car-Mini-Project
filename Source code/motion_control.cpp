#include "motion_control.h"

enum PathIndex
{
	NOPATH_FREE = 0,
	RIGHTPATH_FREE,
	MIDPATH_FREE,
	RIGHT_AND_MIDPATH_FREE,
	LEFTPATH_FREE,
	LEFT_AND_RIGHTPATH_FREE,
	LEFT_AND_MIDPATH_FREE,
	ALLPATHS_FREE
};

/**
	*@note '1' or 'true' = free path while '0' or
	 'false' = obstacle detection
*/
typedef struct
{
	bool left;
	bool middle;
	bool right;
}path_t;
																	 
bool ObstacleDetected(HCSR04& sensor,
											const uint32_t& minObstacleDistance)
{
	uint32_t distance;
	distance = sensor.GetDistance();
	if(distance <= minObstacleDistance)
	{
		return 1;
	}
	return 0;
}

uint8_t GetPathIndex(HCSR04& left,
										 HCSR04& mid,
										 HCSR04& right,
										 const uint32_t& minObstacleDistance)
{
	const uint8_t numberOfPaths = 8;
	const path_t pathArr[numberOfPaths] =
	{
		{0,0,0},{0,0,1},{0,1,0},{0,1,1},
		{1,0,0},{1,0,1},{1,1,0},{1,1,1}
	};
	path_t path = {1,1,1};
	uint8_t pathIndex;
	
	if(ObstacleDetected(left,minObstacleDistance))
	{
		path.left = 0;
	}
	if(ObstacleDetected(mid,minObstacleDistance))
	{
		path.middle = 0;
	}
	if(ObstacleDetected(right,minObstacleDistance))
	{
		path.right = 0;
	}
	for(uint8_t i = 0; i < numberOfPaths; i++)
	{
		if((path.left == pathArr[i].left)&&
			 (path.middle == pathArr[i].middle)&&
		   (path.right == pathArr[i].right))
		{
			pathIndex = i;
			break;
		}
	}
	return pathIndex; 
}

void MoveToFreePath(HCSR04& left,
										HCSR04& mid,
										HCSR04& right,
										const uint32_t& minObstacleDistance,
										Motor& leftMotor,
										Motor& rightMotor)
{
	uint8_t pathIndex = GetPathIndex(left,mid,right,minObstacleDistance);
	
	switch(pathIndex)
	{
		case NOPATH_FREE:
		case MIDPATH_FREE:					
			if(left.GetDistance() <= right.GetDistance())
			{
				rightMotor.Reverse(24000);	
				leftMotor.MoveForward(24000);
				HAL_Delay(500);
			}
			else
			{
				leftMotor.Reverse(24000);
				rightMotor.MoveForward(24000);
				HAL_Delay(500);
			}
			break;
		case RIGHTPATH_FREE:
		case RIGHT_AND_MIDPATH_FREE:	
			leftMotor.MoveForward(16000);
			rightMotor.Reverse(8000);	
			break;
		case LEFTPATH_FREE:
		case LEFT_AND_MIDPATH_FREE:		
			rightMotor.MoveForward(16000);
			leftMotor.Reverse(8000);
			break;
		case LEFT_AND_RIGHTPATH_FREE:
			if(left.GetDistance() <= right.GetDistance())
			{
				leftMotor.MoveForward(16000);
				rightMotor.Reverse(8000);	
			}
			else
			{
				rightMotor.MoveForward(16000);
				leftMotor.Reverse(8000);
			}
			break;
		case ALLPATHS_FREE:
			leftMotor.MoveForward(8000);
			rightMotor.MoveForward(8000);
			break;
	}				
}

