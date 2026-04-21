/*
 * SensorProc.h
 *
 *  Created on: 2025-7-24
 *  Author: Qinghan Yang
 *  Description: Header file for sensor processing functions
 */
#ifndef SENSOR_PROC_H
#define SENSOR_PROC_H

#include "AllHeader.h"

//以下是关于灰度传感器的宏定义
#define SENSOR_COUNT 8 // 定义传感器数量
#define DisSensorToWheel 195 // 灰度传感器到轮子的垂直距离
#define DisSensor 12 // 灰度传感器之间的距离


bool isInTheYaw(float targetYaw, float tolerance); // Check if the current yaw is within a specified tolerance of the target yaw

float Grayscale_Num_To_Theta(int num); // Convert sensor number to angle in degrees
float thetaGrayscale(); // Calculate the angle based on grayscale sensor readings
bool Road_detect(int nummin, int nummax);
bool half_Detect();
bool cross_Roads_Detect();

#endif // SENSOR_PROC_H