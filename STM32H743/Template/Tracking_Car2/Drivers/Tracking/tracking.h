#ifndef _IRTRACKING_H_
#define _IRTRACKING_H_

#include "AllHeader.h"

void car_irtrack(void); //不用PID巡线

void lineWalking_high(void); // 高速巡线
void lineWalking_low(void); // 低速巡线
void lineWalking_core(int16_t speed, float kp, float ki, float kd); // 核心巡线逻辑，参数可配置
float PID_IR_Calc_Custom(int16_t actual_value, float kp, float ki, float
kd); // PID计算函数，参数可配置
void Motion_Car_Control(int16_t V_x, int16_t V_y, int16_t V_z); // 运动控制函数

bool Road_detect(int, int);
bool half_Detect();
bool cross_Roads_Detect();

#endif
