/**
 * @file  Pid.c
 * @brief PID 控制器实现，提供初始化、计算、复位等功能
 */
#include "Pid.h"
#include <stddef.h>

void PID_Init(PIDController *pid, float Kp, float Ki, float Kd, float integral_limit) {
    if (pid == NULL) return;
    pid->config.Kp = Kp;
    pid->config.Ki = Ki;
    pid->config.Kd = Kd;
    pid->config.integral_limit = integral_limit;
    pid->state.error = 0.0f;
    pid->state.sum = 0.0f;
    pid->state.difference = 0.0f;
}

void PID_Reset(PIDController *pid) {
    if (pid == NULL) return;
    pid->state.error = 0.0f;
    pid->state.sum = 0.0f;
    pid->state.difference = 0.0f;
}

void PID_Update(PIDController *pid, float target, float current, float dt) {
    if (pid == NULL || dt <= 0.0f) return;
    float preerror = pid->state.error;
    pid->state.error = target - current;
    pid->state.sum += pid->state.error * dt;
    if (pid->state.sum > pid->config.integral_limit)
        pid->state.sum = pid->config.integral_limit;
    else if (pid->state.sum < -pid->config.integral_limit)
        pid->state.sum = -pid->config.integral_limit;
    pid->state.difference = (pid->state.error - preerror) / dt;
}

float PID_Compute(PIDController *pid) {
    if (pid == NULL) return 0.0f;
    return pid->config.Kp * pid->state.error +
           pid->config.Ki * pid->state.sum +
           pid->config.Kd * pid->state.difference;
}
