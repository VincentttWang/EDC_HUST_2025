#ifndef PID_H
#define PID_H

#include <stdint.h>

typedef struct {
    float Kp;
    float Ki;
    float Kd;
    float integral_limit;
} PIDConfig;

typedef struct {
    float error;
    float sum;
    float difference;
} PIDState;

typedef struct {
    PIDConfig config;
    PIDState state;
} PIDController;

void PID_Init(PIDController *pid, float Kp, float Ki, float Kd, float integral_limit);
void PID_Reset(PIDController *pid);
void PID_Update(PIDController *pid, float target, float current, float dt);
float PID_Compute(PIDController *pid);

#endif /* PID_H */
