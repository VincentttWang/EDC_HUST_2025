#ifndef PID_H
#define PID_H

#include <stdio.h>
#include <stdint.h>

typedef struct{
    float error; // 误差
    float sum; // 积分
    float difference; // 微分
}PIDdata;

typedef struct{
    float Kp; // 比例系数
    float Ki; // 积分系数
    float Kd; // 微分系数
}PIDConfig;


//PID 函数声明
void PID_Init(PIDdata *pid);
void PID_Update(PIDdata *pid, float target, float current, float dt);
float PID_Compute(PIDdata *pid, float Kp, float Ki, float Kd);

#endif