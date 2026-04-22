#ifndef STEP_MOTOR_CTRL_H
#define STEP_MOTOR_CTRL_H

#include "StepMotor.h"
#include "Pid.h"
#include "InitStepMotor.h"
#include "BspCommon.h"

#define YDIS_MOTOR_O 0.0f
#define XDIS_MOTOR_O -0.6f
#define ZDIS_MOTOR_O 1.0f

#define C_PI 3.14159265358979323846f

typedef struct {
    float (*t_to_x)(float t);
    float (*t_to_y)(float t);
    float init_t;
} TargetPositionFunctions;

void PID_SMotor_Cont(void);

void SetTargetCenter(void);
void SetTargetCircle(void);
void SetLaserPosition(void);
void Compute_excur(void);

float getDistance(void);

#endif /* STEP_MOTOR_CTRL_H */
