#include "Init_SMotor.h"

SMotor yawMotor = {0};
SMotor pitchMotor = {0};

void YP_SMotor_Init() {
    SMotor_Init(&yawMotor, YAW_SMOTOR_DIR_PORT, YAW_SMOTOR_DIR_PIN, YAW_SMOTOR_PWM_TIMER, YAW_SMOTOR_PWM_CHANNEL);
    SMotor_Parameters_Init(&yawMotor, YAW_SMOTOR_ANTI_DIR, YAW_SMOTOR_STEP_ANGULAR, YAW_SMOTOR_STEP_DIVISOR);
    SMotor_Init(&pitchMotor, PITCH_SMOTOR_DIR_PORT, PITCH_SMOTOR_DIR_PIN, PITCH_SMOTOR_PWM_TIMER, PITCH_SMOTOR_PWM_CHANNEL);
    SMotor_Parameters_Init(&pitchMotor, PITCH_SMOTOR_ANTI_DIR, PITCH_SMOTOR_STEP_ANGULAR, PITCH_SMOTOR_STEP_DIVISOR);
}

void YP_SMotor_SetSpeed(float yaw_speed, float pitch_speed) {
    SMotor_SetSpeed(&yawMotor, yaw_speed);
    SMotor_SetSpeed(&pitchMotor, pitch_speed);
}

void YP_SMotor_UpdateState() {
    SMotor_UpdateState(&yawMotor);
    SMotor_UpdateState(&pitchMotor);
}

double GetYaw() {
    return yawMotor.state.current_position; // Return the current position of the yaw motor
}

double GetPitch() {
    return pitchMotor.state.current_position; // Return the current position of the pitch motor
}