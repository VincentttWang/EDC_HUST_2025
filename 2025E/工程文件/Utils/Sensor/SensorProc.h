/*
 * SensorProc.h
 *
 *  Created on: 2025-7-24
 *  Author: Qinghan Yang
 *  Description: Header file for sensor processing functions
 */
#ifndef SENSOR_PROC_H
#define SENSOR_PROC_H

#include "Cont_SMotor.h"

//以下是关于视觉处理部分的宏定义
#define PAPERWIDE 315 //单位mm
#define PAPERHIGHT 212 //单位mm
#define CIRCLERADIUS 60 //单位mm

#define XDISM 500
#define YDIS 500
#define LINELENGTH 1000

//以下是关于灰度传感器的宏定义
#define SENSOR_COUNT 8 // 定义传感器数量
#define DisSensorToWheel 195 // 灰度传感器到轮子的垂直距离
#define DisSensor 12 // 灰度传感器之间的距离

Coordinate paper_to_camera(Coordinate paper); // Convert paper coordinates to camera coordinates
Coordinate get_target_coordinate(); // Get the target coordinate based on edge and distance

float Grayscale_Num_To_Theta(int num); // Convert sensor number to angle in degrees
float thetaGrayscale(); // Calculate the angle based on grayscale sensor readings
bool Road_detect(int nummin, int nummax);
bool half_Detect();
bool cross_Roads_Detect();
bool empty_Detect(); // Check if no road is detected
bool centerDetect();

#endif // SENSOR_PROC_H