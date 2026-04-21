#ifndef INIT_SMOTOR_H
#define INIT_SMOTOR_H

#include "SMotor.h"
#include "main.h"
#include "tim.h"

#define YAW_SMOTOR_PWM_TIMER &htim5
#define YAW_SMOTOR_PWM_CHANNEL TIM_CHANNEL_2
#define YAW_SMOTOR_DIR_PORT DIR2_GPIO_Port
#define YAW_SMOTOR_DIR_PIN DIR2_Pin
#define YAW_SMOTOR_ANTI_DIR GPIO_PIN_SET
#define YAW_SMOTOR_STEP_ANGULAR 1.8f
#define YAW_SMOTOR_STEP_DIVISOR 32.0f // 32 steps per revolution

#define PITCH_SMOTOR_PWM_TIMER &htim5
#define PITCH_SMOTOR_PWM_CHANNEL TIM_CHANNEL_1
#define PITCH_SMOTOR_DIR_PORT DIR_GPIO_Port
#define PITCH_SMOTOR_DIR_PIN DIR_Pin
#define PITCH_SMOTOR_ANTI_DIR GPIO_PIN_RESET
#define PITCH_SMOTOR_STEP_ANGULAR 1.8f
#define PITCH_SMOTOR_STEP_DIVISOR 32.0f // 32 steps per revolution

void YP_SMotor_Init();
void YP_SMotor_SetSpeed(float yaw_speed, float pitch_speed);
void YP_SMotor_UpdateState();

double GetYaw();
double GetPitch();

#endif // INIT_SMOTOR_H