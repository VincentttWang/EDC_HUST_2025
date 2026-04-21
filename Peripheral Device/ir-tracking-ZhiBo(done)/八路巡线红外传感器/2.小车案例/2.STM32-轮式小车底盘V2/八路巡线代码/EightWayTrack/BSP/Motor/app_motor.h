#ifndef __APP_MOTOR_H_
#define __APP_MOTOR_H_

#include "AllHeader.h"

//小车底盘电机间距之和的一半 Half of the sum of the distances between the chassis and motors of the trolley
#define Car_APB          				(188.0f)//  (228+148)/2

void Set_Motor(int MOTOR_TYPE);
void Motion_Car_Control(int16_t V_x, int16_t V_y, int16_t V_z);
void Mecanum_Yaw_Calc(float offset_yaw);
void Get_Odometry(void);



#endif
