/**
 * @file  Tracking.h
 * @brief 巡线算法接口，提供红外循迹 PID 控制与运动控制
 */
#ifndef TRACKING_H
#define TRACKING_H

#include <stdint.h>

void lineWalking_low(void);
void lineWalking_core(int16_t speed, float kp, float ki, float kd);
float PID_IR_Calc_Custom(int16_t actual_value, float kp, float ki, float kd);
void Motion_Car_Control(int16_t V_x, int16_t V_y, int16_t V_z);

#endif /* TRACKING_H */
