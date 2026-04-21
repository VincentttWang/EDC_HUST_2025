/*
 * SensorProc.c
 *
 *  Created on: 2025-7-24
 *  Author: Qinghan Yang
 *  Description: Source file for sensor processing functions
 */
#include "SensorProc.h"

// 以下函数用于处理陀螺仪
bool isInTheYaw(float targetYaw, float tolerance)
{
    // Check if the current yaw is within the specified tolerance of the target yaw
    float currentYaw = getYaw(); // Get the current yaw angle
    float dif = fabs(sumTheta(currentYaw, -targetYaw)); // Calculate the difference between current and target yaw
    // If the absolute difference is less than the tolerance, return true
    if (dif < tolerance || 180 - dif < tolerance) {
        return true; // If within tolerance, return true
    } else {
        return false; // Otherwise, return false
    }
}

//以下函数用于处理灰度传感器

float Grayscale_Num_To_Theta(int num)
{
    // Convert the sensor number to an angle in degrees
    // The angle is calculated based on the sensor's position
    return RAD_TO_DEG(atan(-(num - (SENSOR_COUNT - 1) / 2.0f) * DisSensor / DisSensorToWheel));
}

float thetaGrayscale()
{
    float theta = 0.0f;
    int sum = 0;
    // 计算灰度传感器的偏差而导致的误差值
   for(int i = 0; i < SENSOR_COUNT; i++)
    {
        if(Digital[i] == 0) 
        {
            theta += Grayscale_Num_To_Theta(i);
            sum++;
        }
    }
    return theta / sum;
}

bool Road_detect(int nummin, int nummax)
{
		int sumIRDetect = 0;
		int i = 0;
		while(i < 8){	if(Digital[i]==0) sumIRDetect ++; i++;}
		if(sumIRDetect <= nummax && sumIRDetect >= nummin) 	return 1;
		else return 0;
}
bool half_Detect(){
//三岔路口检测
	return Road_detect(5, 7);
}

bool cross_Roads_Detect(){
//十字路口（丁字路口）检测
	return Road_detect(7, 8);
}