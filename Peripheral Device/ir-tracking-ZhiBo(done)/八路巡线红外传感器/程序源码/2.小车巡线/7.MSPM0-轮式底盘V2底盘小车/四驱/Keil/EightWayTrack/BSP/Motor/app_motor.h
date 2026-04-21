#ifndef __APP_MOTOR_H_
#define __APP_MOTOR_H_

#include "ti_msp_dl_config.h"
#include "app_motor_usart.h"
#include "delay.h"

//小车底盘电机间距之和的一半 Half of the sum of the spacing between the motors of the trolley chassis
#define Car_APB          				(188.0f)//  (228+148)/2

void Set_Motor(int MOTOR_TYPE);
void Motion_Car_Control(int16_t V_x, int16_t V_y, int16_t V_z);


#endif
