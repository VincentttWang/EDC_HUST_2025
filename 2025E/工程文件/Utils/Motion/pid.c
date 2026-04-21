#include "pid.h"

void PID_Init(PIDdata *pid) {
    if (pid == NULL) return;
    pid->error = 0.0f;
    pid->sum = 0.0f;
    pid->difference = 0.0f;
}
void PID_Update(PIDdata *pid, float target, float current, float dt) {
    if (pid == NULL || dt <= 0.0f) return;
    float preerror = pid->error;
    pid->error = target - current;
    pid->sum += pid->error * dt;
    pid->difference = (pid->error - preerror) / dt;
}
float PID_Compute(PIDdata *pid, float Kp, float Ki, float Kd) {
    if (pid == NULL) return 0.0f;
    if (pid->sum > 1000.0f) {
        pid->sum = 1000.0f; // ??????????
    } else if (pid->sum < -1000.0f) {
        pid->sum = -1000.0f; // ????????��
    }
    return Kp * pid->error + Ki * pid->sum + Kd * pid->difference;
}
