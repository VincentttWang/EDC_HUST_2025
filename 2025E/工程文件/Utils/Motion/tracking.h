#ifndef _IRTRACKING_H_
#define _IRTRACKING_H_

#include "AllHeader.h"

void car_irtrack(void); 

void lineWalking_high(void);
void lineWalking_low(void);
void lineWalking_core(int16_t speed, float kp, float ki, float kd);
float PID_IR_Calc_Custom(int16_t actual_value, float kp, float ki, float
kd);
void Motion_Car_Control(int16_t V_x, int16_t V_y, int16_t V_z);

#endif
